/**
 * @file
 * 
 * File with shell functions-- printing prompt, executing process including pipes and redirects, background jobs, and built ins. 
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "debug.h"
#include "history.h"
#include "token.h"

#ifndef ARG_MAX
#define ARG_MAX 5000
#endif

/**
* Command line struct
*/
struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

void terminate_process(pid_t p);
bool built_ins(char *args[], char *line);
void execute (char *line);
int execute_pipeline(struct command_line *cmds);
void background_jobs(char *args[], pid_t p);

//Global Variables
bool executing;
unsigned int cmd_id = 0;
int jobs_index = 0;
int status = 0;
/**
* Struct to store background job information
*/
struct job 
{
    pid_t pid;
    char *cmd;
};
struct job job_list[10];


/**
*  Method to print prompt, supports emojis
*/
void print_prompt(int i, int status)
{
    //emoji
    //char sad[5] = {0xF0, 0x9F, 0xA4, 0xAE, '\0'};
    char emoji[5] = {0xF0, 0x9F, 0x99, 0x82, '\0'};
    if (status!=0)
    {
        char sad[5] = {0xF0, 0x9F, 0xA4, 0xAE, '\0'};
        strcpy(emoji, sad);
    }

    //get username and hostname
    char *name = getlogin();
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, sizeof hostname);
    char cwd[PATH_MAX];
    char cwd_new[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    if (startsWith("/home", cwd))
    {
        strcpy(cwd_new, "~");
    }

    int tokens = 0;
    char *next_tok = cwd;
    char *curr_tok;
    while ((curr_tok = next_token(&next_tok, "/")) != NULL) 
    {
        if (tokens > 1)
        {
            strcat(cwd_new, curr_tok);
        }
        tokens++;
    }

    if (startsWith("~", cwd_new))
    {
        printf ("[%s]-[%d]-[%s@%s: %s] ", emoji, i, name, hostname, cwd_new);
    }
    else 
    {
        printf ("[%s]-[%d]-[%s@%s: %s] ", emoji, i, name, hostname, cwd);
    }
    
    fflush(stdout);
}
/**
* Execute method that executes line using execvp
*/
void execute (char *line)
{
    char *args[ARG_MAX];
    char *next_tok = strdup(line);
    char *ptr_backup = next_tok;
    char *curr_tok;
    int tok = 0;
    int background = 0;

    while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL && tok < ARG_MAX)
    {
        if (curr_tok[0] == '#')
        {
            break;
        }
        //expand environment variables
        if (curr_tok[0] == '$')
        {
            char* new_str = expand_var(curr_tok);
            curr_tok = new_str;
        }
        if (curr_tok[0] == '&')
        {
            background = 1;
            break;
        }

        args[tok++] = curr_tok;
    }

    args[tok] = '\0';
    
    if (args[0] == 0)
    {
        free(ptr_backup);
        return;
    }

    if (built_ins(args, line))
    {
        free(ptr_backup);
        return;
    }

    struct command_line cmds[tok];

    int num_cmds = 0;

    char **command = &args[0];

    for (int x = 0; x < tok; x++)
    {
        if (strcmp(args[x], "|")==0)
        {
            args[x] = NULL;
            cmds[num_cmds].tokens = command;
            cmds[num_cmds].stdout_pipe = true;
            cmds[num_cmds].stdout_file = NULL;
            command = &args[x+1];
            num_cmds++;
        }
        else if (strcmp(args[x], ">") == 0)
        {
            args[x] = NULL;
            cmds[num_cmds].tokens = command;
            cmds[num_cmds].stdout_file = args[x+1];
            break;
        } 
        else
        {
            cmds[num_cmds].tokens = command;
            cmds[num_cmds].stdout_file = NULL;
        }
    }
    //set last character of last command tokens to null
    cmds[num_cmds].stdout_pipe = false;

    executing = true;
    pid_t child = fork();
    if (child == -1)
    {
        perror("Fork did not work");
    }
    else if (child == 0)
    {
        //child process 
        int ret = execute_pipeline(cmds);
        if (ret != 0)
        {
            perror("execvp");
        }
        //close stdin, stdout, stderror
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        exit(EXIT_FAILURE);
    }
    else 
    {
        if (background == 1) 
        {
            background_jobs(args, child);
        }
        else
        {
            //parent
            executing = false;
            int stat;
            waitpid(child, &stat, 0);
        }
    }
    free(ptr_backup);
}
/**
* Method to check if line is a built in function
*/
bool built_ins(char *args[], char *line)
{
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1]!=NULL)
        {
            if (chdir(args[1])!=0)
            {
                perror("chdir");
            }
            return true;
        }
        else
        {
            //chdir to home
            struct passwd *pw = getpwuid(getuid());
            const char *homedir = pw->pw_dir;
            if (chdir(homedir)!=0)
            {
                perror("chdir");
            }
            status = 0;
            return true;
        }
    }

    if (strcmp(args[0], "!!") == 0)
    {
        //rerun last command
        struct history_entry *temp;
        temp = get_last();
        if (temp != NULL && temp->line!= NULL)
        {
            execute(temp->line);
            return true;
        }
    }
    else if (startsWith("!", line))
    {
        //tokenize and run by cmd_id or by cmd
        line = next_token(&line, " !\n\t\r");
        if (atoi(line)!=0)
        {
            struct history_entry *temp;
            temp = get_entry(atoi(line));
            if (temp != NULL && temp->line != NULL)
            {
                execute(temp->line);
                return true;
            }
        }
        else if (strlen(line) != 0)
        {
            struct history_entry *temp;
            temp = get_entry_line(line);
            if (temp != NULL && temp->line != NULL)
            {
                execute(temp->line);
                return true;
            }
        }
    }

    if (strcmp(args[0], "setenv") == 0)
    {
        if (args[2] != NULL) 
        {
            setenv(args[1], args[2], true);
            return true;
        }
    }

    if (strcmp(args[0], "jobs") == 0)
    {
        for (int j = 0; j < jobs_index; j++) 
        {
            printf("%d %.5s %s", job_list[j].pid, job_list[j].cmd, &(job_list[j].cmd[strlen (job_list[j].cmd) - 6]));
        }
        return true;
    }

    if (strcmp("history", args[0]) == 0)
    {
        add_entry(cmd_id, line);
        cmd_id++;
        print_history();
        return true;
    }

    if (strcmp("exit", args[0]) == 0)
    {
        //free everything
        free_hist();
        exit(EXIT_SUCCESS);
    }
    return false;
}

/**
* Method that handles sigint
*/
void sigint_handler(int signo) 
{
    if (isatty(STDIN_FILENO)) 
    {
        if (!executing) 
        {
            printf ("\n");
            print_prompt(cmd_id, status);
        }
        fflush(stdout);
    }
}

/**
* Method to handle sigchild
*/
void sigchild_handler(int signo) 
{
    //child pid
    pid_t child;
    int stat1;
    while ((child = waitpid(-1, &stat1, WNOHANG)) > 0) 
    {
        //Terminate child process with pid
        terminate_process(child);
    }
}

/**
* Method to support background jobs
*/
void background_jobs(char *args[], pid_t p)
{
    //sigchild signal handler
    signal(SIGCHLD, sigchild_handler);
    //create a new job
    struct job *temp_job = (struct job*)malloc(sizeof(struct job));
    temp_job->pid = p;

    int token = 1;
    char *cmd = args[0];

    while (args[token]!=NULL)
    {
        strcat(cmd, args[token]);
        strcat(cmd, " ");
        token++;
    }
    if (cmd == NULL)
    {
        return;
    }
    //add job to job struct array
    else
    {
        strcat(cmd, "&\n");
        temp_job->cmd = cmd;
        job_list[jobs_index].cmd = temp_job->cmd;
        job_list[jobs_index].pid = temp_job->pid;
        jobs_index++;
    }
    //free temp job
    free (temp_job);
}

/**
* Function to terminate process by pid
*/
void terminate_process(pid_t p)
{
    int i = 0;
    for (i = 0; i < jobs_index; i++) 
    {
        if (job_list[i].pid == p) 
        {
            break;
        }
    }
    //find pid in list
    if (i < jobs_index) 
    {
        //decrement size of jobs list
        jobs_index--;
        //Move all elements one space ahead
        for (int j = i; j < jobs_index; j++) 
        {
            job_list[j] = job_list[j+1];
        }
    }
}


/**
* Function to execute pipeline
* Parameter: array of command line structs
*/
int execute_pipeline(struct command_line *cmds)
{
     while (cmds->stdout_pipe==true)
    {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            return -1;
        }
        pid_t pid = fork();
        //if pid == -1
        if (pid == 0)
        {
            //child
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            //execvp tokens[0] and tokens[1]
            int ret = execvp(cmds->tokens[0], cmds->tokens);
            if (ret == -1)
            {
                perror("execvp");
                return -1;
            }
        }
        else
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
        }
        cmds++;
    }
    if (cmds->stdout_file!=NULL)
    {
        int fd_out = open(cmds->stdout_file, O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
            perror("open");
            return -1;
        }
        dup2(fd_out, STDOUT_FILENO);
        int ret = execvp(cmds->tokens[0], cmds->tokens);
        if (ret == -1)
        {
            perror("execvp");
            return -1;
        }
        close (fd_out);
    }
    else
    {
        int ret = execvp(cmds->tokens[0], cmds->tokens);
        if (ret == -1)
        {
            perror("execvp");
            return -1;
        }
    }
    return 0;
}

/**
* Main method that reads input or supports scripting mode and executes commands
*/
int main(void)
{
    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");
    //print_history();

    //set up signal handler
    signal(SIGINT, sigint_handler); 
    while (true)
    {
        if (isatty(STDIN_FILENO)) 
        {
            print_prompt(cmd_id, status);
        }
        char *line = NULL;
        size_t line_sz = 10000;
        int gl = getline(&line, &line_sz, stdin);
        if (gl == -1)
        {
            free(line);
            break;
        }

        char *line_dup = strdup(line);
        char *pt_backup = line;

        execute(line);
        if (strcmp(line_dup, "\n") != 0 && strcmp(line_dup, "history\n") != 0)
        {
            add_entry(cmd_id, line_dup);
            cmd_id++;
        }
        free(pt_backup);
        free(line_dup);
    }
    return 0;
}
