#include <fcntl.h>    //open(), close()
#include <unistd.h>   //read(), lseek(), ssize_t
#include <stdio.h>    //fileno(), stderr
#include <sys/mman.h> //mmap(), munmap()
#include <errno.h>    //errno
#include <string.h>   //strerror()
#include "my_c.h"     //my_write(), concat()

typedef struct {
 char number[6];
 char place[25];
 char newline;
} entry_t;

/* Close the file, if successful do nothing, otherwise print an error message. */
void close_file(int fd, char *filename)
{
  char *temp;
  if (close(fd) < 0) {
    temp = concat("Error closing file \"", filename);
    temp = concat(temp, "\": ");
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    my_write(fileno(stderr), temp, str_len(temp));
  }
}

/* Check that there are only six digits being point to. */
int only_digits(char *s)
{
  char count = 0;
  for (char c=*s; c >= '0' && c <= '9'; c=*++s, count++);
  return count == 6 ? 1 : 0;
}

/* Check if s1 and s2 are the same, if the first character are the same we return 0 otherwise its difference. */
int compare_entries(char *s1, char *s2)
{
  char *t1, *t2;

  for (t1 = s1, t2 = s2; *t1 && (*t1 == *t2); t1++, t2++);

  if ((t1 - s1) == 6)
    return 0;
  return *t1 - *t2;
}

/* Perform binary search on the dictionary (seekable file mapped to memory with a struct that match the content of each line)
 * to find the line with a phone number that match the one passed.
 */
char *lookup_time(entry_t dict[], ssize_t num_entries, char *number)
{
  ssize_t l, r, m;
  int cmp;

  l = (ssize_t) 0;
  r = num_entries - ((ssize_t) 1);

  while (l <= r) {
    m = (l + r) / ((ssize_t) 2);
    cmp = compare_entries(dict[m].number, number);
    if (cmp == 0)
      return dict[m].place;

    if (cmp < 0)
      l = m + ((ssize_t) 1);
    else
      r = m - ((ssize_t) 1);
  }

  return NULL;
}

/* Search for the last character that is not a white space and print from the first character to
 * the last non-white space
 */
void print_trimmed(char *ptr)
{
  char *last_no_space = ptr;

  for (int i = 0; i < 25; i++) {
    if (ptr[i] != ' ')
      last_no_space = &ptr[i];
  }

  my_write(1, copy_str(ptr, (size_t) (last_no_space-ptr+1)), (size_t) (last_no_space-ptr+1));
}

int main(int argc, char **argv)
{
  char *filename;
  char *number;
  char *temp;
  int fd;
  char *ptr = NULL;
  off_t lseek_res;
  size_t file_size;
  char *place;

  //if first argument is not given or if first argument have anything that is not a number
  if ((argc < 3) || !only_digits(argv[2])) {
    temp = "Enter a North American phone number prefix with only the first 6 numbers.\n";
    my_write(1, temp, str_len(temp));
    return 1;
  }

  filename = argv[1];
  number = argv[2];

  fd = open(filename, O_RDONLY);

  if (fd < 0) { //If file was unsuccessfully opened
    temp = concat("Error opening file \"", filename);
    temp = concat(temp, "\": ");
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    my_write(fileno(stderr), temp, str_len(temp));
    return 1;
  }

  lseek_res = lseek(fd, (off_t) 0, SEEK_END);

  if (lseek_res == (off_t) -1) {  //If we couldn't seek to the end of the file
    temp = concat("Error seeking file \"", filename);
    temp = concat(temp, "\": ");
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    my_write(fileno(stderr), temp, str_len(temp));
    close_file(fd, filename);
    return 1;
  }

  file_size = (size_t) lseek_res;

  //If the size of the file is not divisible by the size of our struct, meaning that
  //the format is incorrect or that the file is not seekable
  if ((file_size % sizeof(entry_t)) != ((size_t) 0)) {
    temp = concat("The file \"", filename);
    temp = concat(temp, "\" is not properly formatted.\n");
    my_write(fileno(stderr), temp, str_len(temp));
    close_file(fd, filename);
    return 1;
  }

  ptr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);

  close_file(fd, filename); //We don't need the file anymore

  if (ptr == MAP_FAILED) {  //If mapping the file to memory failed
    temp = concat("Error mapping file \"", filename);
    temp = concat(temp, "\" in memory: ");
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    my_write(fileno(stderr), temp, str_len(temp));
    return 1;
  }

  //Find location of number on file
  place = lookup_time((entry_t *) ptr, ((ssize_t) (file_size / sizeof(entry_t))), number);

  //If line was not found, print error message and die
  if (place == NULL) {
    temp = concat("Code didn't match a North American phone number with prefix ", number);
    temp = concat(temp, " in nanpa.\n");
    my_write(fileno(stderr), temp, str_len(temp));
  } else {
    temp = "Place: ";
    my_write(1, temp, str_len(temp));
    print_trimmed(place);
    my_write(1, "\n", 2);
  }

  //unmap the file from memory
  if (munmap(ptr, file_size) != 0) {  //If unmaping was unsuccessful
    temp = concat("Error unmapping file \"", filename);
    temp = concat(temp, "\" in memory: ");
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    my_write(fileno(stderr), temp, str_len(temp));
    return 1;
  }

  return 0;
}

