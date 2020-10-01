# Project 2: Command Line Shell

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html

Writer: Ankita Khatri
University of San Francisco
CS 326: Operating Systems

About This Project:
In this project I created my own Command Line Shell called ./crash. The outermost layer of the operating system kernel is called the shell. In Unix-based systems, the shell is generally a command line interface. Most Linux distributions ship with bash as the default (there are several others: csh, ksh, sh, tcsh, zsh). 

What is a Command Line Shell:
A shell is a computer program that produces a command line interface that allows you to enter commands and control your computer. I created a command line shell that supports basic functions like built in commands (cd, comments, !, !!, and scripting mode), background jobs, signal handling, variable expansion, history, and pipes and redirection.

To compile and run:

```bash
make
./crash
```

### Program Output
```bash
shell.c:503:main(): Initializing shell
shell.c:505:main(): Setting locale: en_US.UTF-8
[🙂]-[0]-[akhatri@akhatri-vm: ~P2-ankitakhatri] ls
shell.c:468:execute_pipeline(): EXECUTING TOKENS: ls
''$'\204\300\017\205\244\003'  'H'$'\211\334\353\004\220\353\001\220''H'$'\213''E'$'\310''dH3'$'\004''%('   shell.c	     vgcore.24053   vgcore.24114   vgcore.24185
''$'\204\300\017\205\313\003'   history.c								    shell.o	     vgcore.24060   vgcore.24127   vgcore.24194
 a.out			        history.h								    test-output.md   vgcore.24064   vgcore.24138   vgcore.24204
 comm			        history.h.gch								    tests	     vgcore.24071   vgcore.24147
 crash			        history.o								    token.c	     vgcore.24082   vgcore.24151
 debug.h		       'ls'$'\n'								    token.h	     vgcore.24092   vgcore.24163
 debug.h.gch		        Makefile								    token.h.gch      vgcore.24097   vgcore.24170
 docs			        README.md								    token.o	     vgcore.24107   vgcore.24176
[🙂]-[1]-[akhatri@akhatri-vm: ~P2-ankitakhatri] whoami
shell.c:468:execute_pipeline(): EXECUTING TOKENS: whoami
akhatri
[🙂]-[2]-[akhatri@akhatri-vm: ~P2-ankitakhatri] 
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
