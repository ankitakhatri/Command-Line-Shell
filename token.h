/**
 * @file
 *
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *next_token(char **str_ptr, const char *delim);
bool startsWith(const char *pre, const char *str);
char *expand_var(char *str);

#endif