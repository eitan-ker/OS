#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

typedef struct history{
    char* command;
    int pid;
    char* status;
} history;

typedef struct specialdir{
    char currDir[1024];
    char lastDir[1024];
    char homeDir[1024];

} specialdir;

int cd(char *command[], int iter, specialdir* dirStr);

void exitFunc();

void exec(char *command[]);

int main() {
    // initialization
    char prompt[3] = "> ";
    char input[100];
    char dummy;
    char *command;
    char *temp;
    char *comArr[100];
    int i;

    // initialization of directory struct
    specialdir* dirStr = (specialdir*)malloc(sizeof(specialdir));;
    getcwd(dirStr->homeDir, sizeof(dirStr->homeDir));
    strcpy(dirStr->lastDir, dirStr->homeDir);
    strcpy(dirStr->currDir, dirStr->homeDir);


    // running the shell in the while loop
    while (true) {
        printf("%s", prompt);
        scanf("%[^\n]s", input);
        scanf("%c", &dummy);
        command = strtok(input, " ");
        comArr[0] = command;
        temp = strtok(NULL, " ");
        i = 1;
        // inserting all arguments
        while (temp != NULL) {
            comArr[i] = temp;
            i++;
            temp = strtok(NULL, " ");
        }
        comArr[i] = NULL;
        i--;
        if (!strcmp(comArr[0], "jobs")) {
            // jobs func
        } else if (!strcmp(comArr[0], "history")) {
            // history func
        } else if (!strcmp(comArr[0], "cd")) {
            // cd func
            cd(comArr, i, dirStr);
        } else if (!strcmp(comArr[0], "exit")) {
            // exit func
            exitFunc();
        } else {
            // exec func
            exec(comArr);
        }
    }
    return 0;
}

int cd(char *command[100], int iter, specialdir* dirStr) {
    int temp = 0;
    char tempDir[1024]; // save last in case cd didn't work
    printf("%d\n", (int) getpid());

    //             currentHistory[numOfCommand]->pid = (int)getpid();

    if (iter >= 2) {
        printf("%d\n", (int) getpid());
        fprintf(stderr, "Error: Too many arguments");
    } else {
        strcpy(tempDir, dirStr->lastDir);
        strcpy(dirStr->lastDir, dirStr->currDir); // last getting the current before change
        if (!strcmp(command[iter], "~")) {
            temp = chdir(dirStr->homeDir);
            strcpy(dirStr->currDir, dirStr->homeDir);
        } else if (!strcmp(command[iter], "-")) {
            temp = chdir(tempDir);
            strcpy(dirStr->currDir, tempDir);
        } else if (!strcmp(command[iter], "..")) {
            temp = chdir(command[iter]);
            getcwd(dirStr->currDir, sizeof(dirStr->currDir));
        } else {
            temp = chdir(command[iter]);
            strcpy(dirStr->currDir, command[iter]);
        }
    }
    if (temp == -1) {
        strcpy(dirStr->lastDir, tempDir);
        strcpy(dirStr->currDir, dirStr->lastDir); // in case cd didn't work go back to last dir
        printf("%d\n", (int) getpid());
        fprintf(stderr, "Error in system call");
        // do i need to return if temp = -1
    }
    return temp;
}

void exitFunc() {
    printf("%d\n", (int) getpid());
    exit(0);
}

void exec(char *command[]){
        //‪execv‬‬
        pid_t pid;
        if ((pid = fork()) == 0){
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%d\n",(int)getpid());

            execvp(command[0],command);
        }
        else {
            signal(SIGCHLD,SIG_IGN);
        }
};