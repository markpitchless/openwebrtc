
#include "cocoon_utils.h"

char* prompt_line(const char *prompt) {
  g_print("%s", prompt);

  int size = 64; // initial str size to store input
  char* str = malloc(size*sizeof(char));
  if (str == NULL) {
    return NULL; // out of memory
  }
  char c = '\0';
  int i = 0;
  do {
    c = getchar();
    if (c == '\r' || c == '\n' || c == EOF) {
        c = '\0'; // end str if user hits <enter>
    }
    if (i == size) {
        size *= 2; // duplicate str size
        str = realloc(str, size*sizeof(char));; // and reallocate it
        if (str == NULL) {
          free(str); // the memory block is not deallocated by realloc
          return NULL; // out of memory
        }
    }
    str[i++] = c;
  } while (c != '\0');
  str = realloc(str, i); // trim memory to the str content size
  return str;
}
