/**
 * @file
 *
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#ifndef _HISTORY_H_
#define _HISTORY_H_

#define HIST_MAX 100
/**
* Struct for history entry 
*/
struct history_entry {
    int cmd_id;
    char *line;
    /* What else do we need here? Note: you aren't required to use this struct,
     * it is only here for demonstration purposes. */
};

/*Function prototypes*/
void add_entry(unsigned int cmd_id, char *line);
struct history_entry *get_entry(int cmd_id);
struct history_entry *get_entry_line(char *line);
struct history_entry *get_last(void);
void print_history();
void free_hist();

#endif
