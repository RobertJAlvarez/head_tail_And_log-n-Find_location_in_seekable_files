#include <fcntl.h>      //open(), close()
#include <unistd.h>     //read(), ssize_t
#include <stdio.h>      //fileno(), stderr
#include <errno.h>      //errno
#include <string.h>     //strerror()
#include "linked_list.h"
#include "my_c.h"

#define BUFFER_LEN (4096)

/* Free the space used by the Linked list, Items, strings in Items, close the file
 * and print an error message of why this is been done.
 */
void close_prog(List *LL, int fd, char str[])
{
  char *temp;

  if (*str != '\0') {
    my_write(fileno(stderr), str, str_len(str));
  }
  //deallocate everything that has been allocated
  if (fd != 0) {
    if (close(fd) < 0) {
      temp = concat("Error closing file: ", strerror(errno));
      temp = concat(temp, "\n");
      my_write(fileno(stderr), temp, str_len(temp));
    }
  }
  free_history(LL);
}

int main(int argc, char **argv)
{
  List *LL = NULL;
  char *file = NULL;
  char *temp = NULL;
  char buffer[BUFFER_LEN];
  int k;
  ssize_t read_res;
  size_t remaining_bytes;
  size_t bytes_read;
  size_t line_bytes;
  int fd;

  //Find the value of k if -n was provided and save file names if any
  k = 10;   //Assume there was no -n # provided
  for (int i = 1; i < argc; i++) {
    //if (str_cmp(argv[i],n_char) == 0) { // Check if the argument is -n
    if (str_cmp(argv[i],(char *)"-n") == 0) { // Check if the argument is -n
      if (++i >= argc) {  //If there is nothing after -n
        temp = "tail: option requires an argument -- n\nusage: tail [-n #] [file]\n";
        my_write(fileno(stderr), temp, str_len(temp));
        return 1;
      }
      //Get what is after -n
      k = str_to_int(argv[i]);
      if (k == 0) { //If 0, a negative number or a non-number got input after -n
        temp = "head: illegal line count -- ";
        temp = concat(temp,argv[i]);
        temp = concat(temp, "\n");
        my_write(fileno(stderr), temp, str_len(temp));
        return 1;
      }
    } else {
      file = argv[i];
    }
  }

  //Initialize linked list to hold at most k lines
  LL = init_LL(k);

  //Get file descriptor
  fd = (file != NULL ? open(file, O_RDONLY) : 0);

  //Keep reading until the file hits the end-of-file condition
  while (1) {
    //Try to read into the buffer, up to sizeof(buffer) bytes
    read_res = read(fd, buffer, sizeof(buffer));

    //Handle the return values of the read system call

    //If the returned value is zero, we are done, as this is end-of-file
    if (read_res == ((ssize_t) 0)) break;
    //If the returned value is negative, we have an error and we die
    if (read_res < ((ssize_t) 0)) {
      temp = "Error reading: ";
      temp = concat(temp, strerror(errno));
      temp = concat(temp, "\n");
      close_prog(LL, fd, temp);
      return 1;
    }

    //Here we known that read_res in positive.
    remaining_bytes = (size_t) read_res;

    bytes_read = (size_t) 0;
    while (remaining_bytes > (size_t) 0) {
      line_bytes = get_line_bytes(&buffer[bytes_read], remaining_bytes);
      temp = copy_str(&buffer[bytes_read],line_bytes);
      //If space to copy new line couldn't be allocate
      if (temp == NULL) {
        close_prog(LL, fd, "Could not allocate any more memory.\n");
        return 1;
      }
      if (add_item(LL, temp, line_bytes) < (ssize_t) 0) {     //Space to add another item couldn't be allocated
        close_prog(LL, fd, "Could not allocate any more memory.\n");
        return 1;
      }
      remaining_bytes -= line_bytes;
      bytes_read += line_bytes;
    }
  }

  if (print_lines(LL) < (ssize_t) 0) {
    temp = "Error writing: ";
    temp = concat(temp, strerror(errno));
    temp = concat(temp, "\n");
    close_prog(LL, fd, temp);
    return 1;
  }

  close_prog(LL, fd, "");
  return 0;
}

