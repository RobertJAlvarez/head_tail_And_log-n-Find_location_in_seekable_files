#ifndef _MY_C_
#define _MY_C_

/*Compare two strings, return > 0 if s1 > s2, < 0 if s1 < s2, 0 if s1 == s2*/
int str_cmp(char *s1, char *s2);

int str_to_int(char *s);

/* Returns a freshly allocated new zero-terminated string containing <len> chars from <inStr> */
char *copy_str(char *inStr, size_t len);

size_t str_len(char *s);

size_t get_line_bytes(char *buf, size_t rem);

char *concat(char *s1, char *s2);

/* Call write() until all bytes are written or until
 * an error occurs. Returns 0 on success and -1 on failure
 */
int my_write(int fd, const char *buf, size_t bytes);

#endif
