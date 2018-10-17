#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void freeStuff(char **arr, int n){
  int i;
  for(i = 0; i < n; i++){
    free(arr[i]);
  }
  free(arr);
}

void sigtstp_handler(int sig){
  char msg[] = "caught sigstp\nCS361 >";
  write(1, msg, sizeof(msg));
  return;
}

void sigint_handler(int sig){
  char msg[] = "caught sigint\nCS361 >";
  write(1, msg, sizeof(msg));
  return;
}

void forkChild(char *args[], int n){
  int status;
  int fd;
  int i;
  char *word;
  int pid = fork();
  int split = 0;

  if(pid == 0){
    for(i = 0; i < n; i++){
      /*
      if(strncmp(args[i], ";", 2) == 0){
        split = 1;
        char* firstArr;
        char* secondArr;
        int a = i;
        for (0; a<i; a++) {
          firstArr[a] = args[a];
        }
        int b = 0;
        for (a=i+1; args[a] == NULL; a++){
          secondArr[b] = args[a];
          b++;
        }
        execvp(firstArr[0], firstArr);
        execvp(secondArr[0], secondArr);

        break;
      }else*/ if(strncmp(args[i], ">", 1) == 0){
        args[i] = (char *) 0;
        fd = open(args[i + 1], O_RDWR|O_CREAT, S_IWUSR|S_IRGRP| S_IROTH);
        dup2(fd, 1);
        close(fd);
        break;
      }else if(strncmp(args[i], "<", 1) == 0){
        args[i] = (char *) 0;
        fd = open(args[i+1], O_RDONLY);
        dup2(fd, 0);
        close(fd);
        break;
      }

    }
    if (split == 0)
      execvp(args[0], args);
    exit(0);

  }
  else{
    wait(&status);
    printf("pid:%d", getpid());
    printf(" status:%d\n", WEXITSTATUS(status));
    //printf("Exit: %d\n", status);
  }
}

int main(int argc, char **argv) {

  while (1) {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    char *args[20];
    char line[500];
    int i = 0;
    printf("CS361 >"); //
    fgets(line, 500, stdin);

    char *word = strtok(line, " ");

    while(word){
      if(strncmp(word, "exit", 4) == 0 && i == 0){
        exit(0);
      }
      
      args[i] = (char *) malloc(sizeof(char* )* 100);
      strcpy(args[i], word);
      word = strtok(NULL, " ");
      i++;
    }
    strtok(args[i-1], "\n");
    args[i] = (char *) malloc(sizeof(char* )* 100);
    args[i] = (char *)0;

    forkChild(args, i);
    //freeStuff(args, i);

  }
  return 0;
}
