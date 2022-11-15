#include <fcntl.h>  //open(), close()
#include <unistd.h> //read(), ssize_t
#include <stdio.h>  //fileno(), stderr
#include <errno.h>  //errno
#include <string.h> //strerror()
#include "my_c.h"

#define BUFFER_LEN (4096)

int main(int argc, char **argv)
{
  char buffer[BUFFER_LEN];
  char *file = NULL;
  char *temp;
  int k;
  int n_lines_printed;
  ssize_t read_res;
  size_t remaining_bytes;
  size_t bytes_written;
  size_t line_bytes;
  int fd;

  //Find the value of k if -n was provided and save file names if any
  k = 10;   //Assume there was no -n # provided
  for (int i = 1; i < argc; i++) {
    if (str_cmp(argv[i],"-n") == 0) { // Check if the argument is -n
      if (++i >= argc) {  //If there is nothing after -n
        temp = "head: option requires an argument -- n\nusage: head [-n lines] [file]\n";
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

  //Get file descriptor
  fd = (file != NULL ? open(file, O_RDONLY) : 0);

  //We keep on reading until the file hits the end-of-file condition
  n_lines_printed = 0;
  while (n_lines_printed < k) {
    //Try to read into the buffer, up to sizeof(buffer) bytes
    read_res = read(fd, buffer, sizeof(buffer));

    //Handle the return values of the read system call

    //If the returned value is zero, we are done, as this is end-of-file
    if (read_res == ((ssize_t) 0)) break;
    //If the returned value is negative, we have an error and we die
    if (read_res < ((ssize_t) 0)) {
      //Display the appropriate error message and die
      temp = "Error reading: ";
      temp = concat(temp, strerror(errno));
      temp = concat(temp, "\n");
      my_write(fileno(stderr), temp, str_len(temp));
      return 1;
    }

    //Here we known that read_res in positive.
    remaining_bytes = (size_t) read_res;

    //If buffer was filled but we didn't got to the end of the line
    if ((remaining_bytes == sizeof(buffer)) && (buffer[remaining_bytes-1] != '\n'))
      n_lines_printed--;

    bytes_written = (size_t) 0;
    while ((remaining_bytes > (size_t) 0) && (n_lines_printed < k)) {
      line_bytes = get_line_bytes(&buffer[bytes_written], remaining_bytes);
      //We need to write all the remaining_bytes bytes from buffer to standard output
      if (my_write(1, &buffer[bytes_written], line_bytes) < 0) {
        //Display the appropriate error message and die
        temp = "Error writing: ";
        temp = concat(temp, strerror(errno));
        temp = concat(temp, "\n");
        my_write(fileno(stderr), temp, str_len(temp));
        return 1;
      }
      remaining_bytes -= line_bytes;
      bytes_written += line_bytes;
      n_lines_printed++;
    }
  }

  if (fd != 0) {  //If file is not zero it means we read information from file so we try to close it
    if (close(fd) < 0) {  //If closing is successful
      temp = concat("Error closing file \"", file);
      temp = concat(temp, "\": ");
      temp = concat(temp, strerror(errno));
      temp = concat(temp, "\n");
      my_write(fileno(stderr), temp, str_len(temp));
    }
  }

  return 0;
}

