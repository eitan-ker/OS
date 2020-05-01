#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

typedef struct history {
    char *command;
    int pid;
    char* status;
} history;

typedef struct specialdir {
    char currDir[1024];
    char lastDir[1024];
    char homeDir[1024];

} specialdir;

void printHIstory(history *hisComm[100], int hisCommIter);

void cd(char *command[], int iter, specialdir *dirStr);

void exitFunc();

void exec(char *command[], history *hisComm[100], int hisCommIter);

int main() {
    // initialization
    char prompt[3] = "> ";
    char input[100];
    char dummy;
    char *command;
    char *temp;
    char *comArr[100];
    int i;
    int backFlag = 0; // is the command had &
    history *hisComm[100]; // INPUT of all commands by client
    int hisCommIter = 0; // number of commands counter

    // initialization of directory struct
    specialdir *dirStr = (specialdir *) malloc(sizeof(specialdir));;
    getcwd(dirStr->homeDir, sizeof(dirStr->homeDir));
    strcpy(dirStr->lastDir, dirStr->homeDir);
    strcpy(dirStr->currDir, dirStr->homeDir);


    // running the shell in the while loop
    while (true) {
        printf("%s", prompt);
        scanf("%[^\n]s", input);
        scanf("%c", &dummy);

        hisComm[hisCommIter] = (history *) malloc(sizeof(history));
        hisComm[hisCommIter]->command = (char *) malloc(strlen(input) + 1);
        strcpy(hisComm[hisCommIter]->command, input);
        hisComm[hisCommIter]->pid = (int) getpid();

        command = strtok(input, " ");
        comArr[0] = command;
        temp = strtok(NULL, " ");
        i = 1;
        // inserting all arguments
        while (temp != NULL) {
            if (!strcmp(temp, "&")) {
                temp = strtok(NULL, " ");
                backFlag = 1;
                continue;
            }
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
            printHIstory(hisComm, hisCommIter);
        } else if (!strcmp(comArr[0], "cd")) {
            // cd func
            printf("%d\n", (int) getpid());
            hisComm[hisCommIter]->pid = (int) getpid();
            cd(comArr, i, dirStr);
        } else if (!strcmp(comArr[0], "exit")) {
            // exit func
            exitFunc();
        } else {
            // exec func
            exec(comArr, hisComm, hisCommIter);
        }
        hisCommIter++;
    }
    return 0;
}

void printHIstory(history *hisComm[100], int hisCommIter) {
    int i = 0;
    for (i = 0; i <= hisCommIter; i++) {
        int tempPid = hisComm[i]->pid;
        if (i == hisCommIter) {
            hisComm[i]->status = "RUNNING";
        }
        if (hisComm[i]->status != NULL) {
            if (!kill(tempPid, 0)) {
                printf("%d\n", kill(tempPid, 0));
                hisComm[i]->status = "RUNNING";
            } else {
                hisComm[i]->status = "DONE";
            }
        } else {
            if (!kill(tempPid, 0)) {
                hisComm[i]->status = "RUNNING";
            } else {
                hisComm[i]->status = "DONE";
            }
        }
        printf("%d %s %s\n", hisComm[i]->pid, hisComm[i]->command, hisComm[i]->status);
    }
    hisComm[hisCommIter] = "DONE";
}

void cd(char *command[100], int iter, specialdir *dirStr) {
    int temp = 0;
    char tempDir[1024]; // save last in case cd didn't work
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
}

void exitFunc() {
    printf("%d\n", (int) getpid());
    exit(0);
}

void exec(char *command[], history *hisComm[100], int hisCommIter) {
    //‪execv‬‬
    pid_t pid;
    if ((pid = fork()) == 0) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%d\n", (int) getpid());
        execvp(command[0], command);
    } else {
        hisComm[hisCommIter]->pid = (int)pid;
        signal(SIGCHLD, SIG_IGN);
    }
    usleep(10000);

};