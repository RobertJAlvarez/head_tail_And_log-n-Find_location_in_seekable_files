#include <stdlib.h> //malloc()
#include <unistd.h> //write(), ssize_t

/*Compare two strings, return > 0 if s1 > s2, < 0 if s1 < s2, 0 if s1 == s2*/
int str_cmp(char *s1, char *s2)
{
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int str_to_int(char *s)
{
  int num = 0;
  for (char c = *s; c >= '0' && c <= '9'; c = *++s)
    num = num*10 + c-48;
  return num;
}

size_t str_len(char *s)
{
  char *c;
  for (c = s; *c != '\0'; c++);
  return ((size_t) (c - s));
}

size_t get_line_bytes(char *buf, size_t rem)
{
  size_t n_bytes;
  n_bytes = (size_t) 0;
  while (buf[n_bytes] != '\n' && n_bytes < rem)
    n_bytes += sizeof(char);
  if (buf[n_bytes] == '\n')
    n_bytes += sizeof(char);
  return n_bytes;
}

/* Returns a freshly allocated new zero-terminated string containing <len> chars from <inStr> */
char *copy_str(char *inStr, size_t bytes)
{
  char *new_str = malloc(bytes); //1 extra because of '\0'
  if (new_str == NULL) return NULL; //Not enough space in malloc
  char *temp = new_str; //save starting position of the new string
  bytes++;  //bytes can only be >= 0 because size_t can only hold positive numbers, so we add one and stop at 0
  while (((bytes-=sizeof(char)) > 0) && (*temp++ = *inStr++));
  return new_str;
}

char *concat(char *s1, char *s2)
{
  size_t new_len = str_len(s1) + str_len(s2) - ((size_t) 1);
  char *new_str = malloc(new_len);
  if (new_str == NULL) return NULL; //Not enough space in malloc
  char *temp = new_str;
  while ( (*new_str++ = *s1++) ); //Copy first "string"
  new_str--;  //Go to '\0' to overwrite it
  while ( (*new_str++ = *s2++) ); //Copy second "string"
  return temp;
}

/* Call write() until all bytes are written or until
 * an error occurs. Returns 0 on success and -1 on failure
 */
int my_write(int fd, const char *buf, size_t bytes)
{
  size_t bytes_to_be_written;
  size_t bytes_already_written;
  ssize_t bytes_written_this_time;

  bytes_already_written = (size_t) 0;
  bytes_to_be_written = bytes;
  while (bytes_to_be_written > ((size_t) 0)) {
    bytes_written_this_time = write(fd, &buf[bytes_already_written], bytes_to_be_written);
    if (bytes_written_this_time < ((ssize_t) 0)) //Error
      return -1;
    bytes_to_be_written -= (size_t) bytes_written_this_time;
    bytes_already_written += (size_t) bytes_written_this_time;
  }
  return 0;
}

