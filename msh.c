// The MIT License (MIT)
//
// Copyright (c) 2016 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

// NOTE TO SELF ls counts as an agrument so ls + 9 other arguments
#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports 10 arguments

// Used to print out the history and this will only print 15 of the past commands
#define MAX_HISTORY 15


int main()
{

  char *command_string = (char *)malloc(MAX_COMMAND_SIZE);
  // used to keep track of the array list used for history
  int historyCount = 0;

  // will be used in history and will be used as an array list
  char *history[100];

  while (1)
  {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline. The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      token[i] = NULL;
    }

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;

    char *working_string = strdup(command_string);

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end

    // DID NOT USE THIS YET DONT KNOW IF I WILL NEED TO
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    // char *historyTrack [MAX_NUM_ARGUMENTS];

    // make test_blank

    // if nothing is given in the command line and entered is pressed
    // dont return anything just call the while loop again to print msh>
    // and ask for another input
    if (token[0] == NULL)
    {
      continue;
    }

    // else go in to this part of the loop
    // (basically if there is an argument given no matter if it is vaild or not
    // validity of command will be checked later on)
    else
    {

      // make test_quit
      // make test_exit
      // req 4. Your shell will exit with status zero if the command is “quit” or “exit”.

      // if the first argument of the command line is 'quit' or 'exit' then the
      // it will return a 0 to the while loop there for breaking out the while loop
      if ((strcmp(token[0], "quit") == 0) || (strcmp(token[0], "exit") == 0))
      {
        exit(0);
      }

      // make test_cd

      // if the user wants to change directory and they say cd of inside this
      // not sure if I need to have something if there is nothing after cd
      else if (strcmp(token[0], "cd") == 0)
      {
        // chdir changes the directory in C
        // token[1] is passed in which is the directory that it needs to change to
        chdir(token[1]);

        // historyCD_flag = 1;

        // allocate memory for history
        history[historyCount] = malloc(strlen(token[0]) + 1);
        // have to do string copy can not do history[countHistory] = token[0]
        strcpy(history[historyCount], token[0]);
        // have increment after aollocating becasue historyCount for the next use
        historyCount++;
      }


      // make test_history

      // else statment
      // if the history count is lower than 15 the print out the AL
      /* else (if the history count is higer) delete everything
         in the front until there is only 15 */
      // NOTE TO SELF CALLING A FUNCTION MIGHT BE EASIER BECAUSE YOU HAVE TO DELETE
      // OR HAVE TO IMPLEMENT A LINKED LIST BECASUE THIS NEEDS TO BE A QUEUE (FIFO)

      else if ((strcmp(token[0], "history") == 0))
      {
        // THIS IS USED TO STORE THE TOKEN THAT WAS PASSED IN
        // allocate memory for history
        history[historyCount] = malloc(strlen(token[0]) + 1);
        // have to do string copy can not do history[countHistory] = token[0]
        strcpy(history[historyCount], token[0]);
        // have increment after aollocating becasue historyCount for the next use
        historyCount++;

        // prints the array list if the array list is has 15 or less things inside
        if (historyCount <= MAX_HISTORY)
        {
          // the loop used to print everything in the list
          for (int i = 0; i < historyCount; i++)
          {
            printf("%d : ", i + 1);
            printf("%s\n", history[i]);
          }
        }
        // used if the history is over 15 could not figure out how to delete the first one
        // and move every thing up on the array list
        else
        {
          printf("Will go over 15 on the history\n");

          // the loop used to print the array list
          for (int i = 0; i < historyCount; i++)
          {
            printf("%d : ", i + 1);
            printf("%s\n", history[i]);
          }
        }
      }

      // make test_notfound
      // make test_ls
      // make test_cp
      // make test_paths

      // taken from the execvp.c from useful-examples

      // this one is in the child directory and will exit the child
      // if the inputs are random or wrong
      else
      {
        // make a child and parent
        pid_t pid = fork();

        // check if it a child a child to check if the command is vaild
        if (pid == 0)
        {
          // used to check if the command is vaild
          int ret = execvp(token[0], &token[0]);

          // if its not vaild then print out an error statement
          // and exit the child
          if (ret == -1)
          {

            // req 2. If the command is not supported your shell shall print
            // the invalid command followed by “: Command not found.”
            printf("%s", token[0]);
            printf(": Command not found.\n");

            // you have to exit out of child after invalid command
            // or else you have to exit multiple times at the end
            exit(0);
          }

          else
          {
            // THIS IS USED TO STORE THE TOKEN THAT WAS PASSED IN
            // allocate memory for history
            history[historyCount] = malloc(strlen(token[0]) + 1);
            // have to do string copy can not do history[countHistory] = token[0]
            strcpy(history[historyCount], token[0]);
            // have increment after aollocating becasue historyCount for the next use
            historyCount++;

          }
        }

        // if its not a child (so a parent) have it wait on the child
        // so the parent can clean up after the child
        // the child will be a zombie state for a short amount of time
        else
        {
          int status;
          wait(&status);

          // THIS IS USED TO STORE THE TOKEN THAT WAS PASSED IN
          // allocate memory for history
          history[historyCount] = malloc(strlen(token[0]) + 1);
          // have to do string copy can not do history[countHistory] = token[0]
          strcpy(history[historyCount], token[0]);
          // have increment after aollocating becasue historyCount for the next use
          historyCount++;
        }
      }
    }

    // free the malloc
    // Cleanup allocated memory
    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      if (token[i] != NULL)
      {
        free(token[i]);
      }
    }

    free(head_ptr);
  }

  // free the malloc to save memory
  free(command_string);
  //have to free history
  free(history);

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
