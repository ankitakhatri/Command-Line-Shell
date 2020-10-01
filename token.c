/**
 * @file
 * This file contains functions for string manipulation: next_tok and expand_var
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

/**
 * Demonstrates string tokenization in C using the strspn(3) and strcspn(3)
 * functions. Unlike strtok(3), this implementation is thread safe. The code
 * is based on the following newsgroup post:
 *
 * https://groups.google.com/forum/message/raw?msg=comp.lang.c/ff0xFqRPH_Y/Cen0mgciXn8J
 */

#include <string.h>
#include <stdio.h>
#include "token.h"

/**
 * Retrieves the next token from a string.
 *
 * Parameters:
 * - str_ptr: maintains context in the string, i.e., where the next token in the
 *   string will be. If the function returns token N, then str_ptr will be
 *   updated to point to token N+1. To initialize, declare a char * that points
 *   to the string being tokenized. The pointer will be updated after each
 *   successive call to next_token.
 *
 * - delim: the set of characters to use as delimiters
 *
 * Returns: char pointer to the next token in the string.
 */
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) 
    {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) 
    {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') 
    {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    }
    else 
    {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

/**
 * Expands environment variables that start with a $ using getenv
 *
 * Parameters: The string that starts with $, which is the variable to expand
 *
 * Returns: char pointer to the environment variable.
 */
char *expand_var(char *line)
{
    line = next_token(&line, " $\n");
    char *env_var = getenv(line);
    return env_var;
}

/**
 * Helper function to see if string starts with substring
 *
 * Parameters:
 * - pre: check if str starts with this line
 * - str: check if this line starts with pre
 *
 * Returns: true if successful, false if unsuccessful.
 */
bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
