#ifndef __COCOON_UTILS_H__
#define __COCOON_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <glib.h>

/**
 * Ask the user on stdout to enter a line of text (on stdin), reads chars until
 * user enters a newline or EOF, returns pointer (char*) to entered text.
 * Handles allocating memory for the input (hopefully safely).
 * Returns NULL on out of memory.
 */
// http://stackoverflow.com/questions/25233730/dynamically-prompt-for-string-without-knowing-string-size
char* prompt_line(const char *prompt);

#endif /* __COCOON_UTILS_H__ */
