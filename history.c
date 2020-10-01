/**
 * @file
 * 
 *	File with history functions: add, get_last, get by command id, and get by command prefix. 
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "token.h"

/* Global variables */
struct history_entry hist[HIST_MAX];

int num_entries = 0;
int ind = 0;
int start = 0;
int end = -1;

//if command is !cmd_id, get and return that command
//if command is !!, get and return last command
//handle these commands in built-in functions method in shell.c

/**
* Method to add entry to history array
*
* Parameters:
* - command id: command id number
* 
* - line: line read from stdin or from script
*
* Returns: void
*/
void add_entry(unsigned int cmd_id, char *line)
{
	//need to handle rearranging array once you hit the first 100

	//create line_dup and add it to hist[num]
	char *line_duplicate = strdup(line);

	if (end == HIST_MAX)
	{
		end = end % HIST_MAX;
		start++;
	}

	if (num_entries < HIST_MAX)
	{
		hist[ind].cmd_id = cmd_id;
		hist[ind].line = line_duplicate;
		num_entries++;
		end++;
		ind++;
	}
	else if (num_entries >= HIST_MAX)
	{
		start = (start+1) % HIST_MAX;
		end = (end+1) % HIST_MAX;
		ind = num_entries % HIST_MAX;
		hist[end].cmd_id = cmd_id;
		hist[end].line = line_duplicate;
	}
}

/**
* Get last entry if cmd starts with !!
*/
struct history_entry *get_last(void) 
{
	if (hist[ind-1].line != NULL)
	{	
		return &hist[ind-1];
	}
	return NULL;
}

/**
* Get entry by cmd_id if cmd starts with !
*/
struct history_entry *get_entry(int cmd_id)
{
	if (cmd_id < num_entries - HIST_MAX)
	{
		perror("Command not found");
		return NULL;
	}
	for (int i = 0; i < HIST_MAX; i++)
	{
		if (hist[i].cmd_id == cmd_id && hist[i].line != NULL)
		{
			return &hist[i];
		}
	}
	return NULL;
}

/**
* Get entry by cmd if cmd starts with !
*/
struct history_entry *get_entry_line(char *line)
{
	if (end<start)
	{
		for (int e = end; e >= 0; e--)
		{	
			if (startsWith(line, hist[e].line))
			{
				return &hist[e];
			}
		}
		for (int s = HIST_MAX-1; s >= start; s--)
		{
			if (startsWith(line, hist[s].line))
			{
				return &hist[s];
			}
		}
	}
	else
	{
		for (int i = num_entries-1; i >= 0; i--)
		{
			if (startsWith(line, hist[i].line))
			{
				return &hist[i];
			}
		}
	}
	return NULL;
}

/**
* Method to print last 100 commands
*/
void print_history()
{
	/* This function should print history entries */
	if (end<start)
	{
		for (int x = start; x < HIST_MAX ; x++)
		{
			printf ("%d %s", hist[x].cmd_id, hist[x].line);
		}
		for (int j = 0; j <= end; j++)
		{	
			printf ("%d %s", hist[j].cmd_id, hist[j].line);
		}
	}
	else
	{
		for (int i = 0; i < num_entries; i++)
		{
			printf ("%d %s", hist[i].cmd_id, hist[i].line);
		}
	}
}

/**
* Method to free everything in history array
*/
void free_hist()
{
	for (int h = 0; h < HIST_MAX; h++)
	{
		free (hist[h].line);
	}
}

