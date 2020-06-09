// Eitan Kerzhner
// 205697139

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>

typedef struct history {
    char *command;
    int pid;
    char *status;
} history;

typedef struct specialdir {
    char currDir[1024];
    char lastDir[1024];
    char homeDir[1024];

} specialdir;

void printHisJob(history *hisComm[100], int hisCommIter, int backFlag, int hisJobsFlag);

void cd(char *command[], int iter, specialdir *dirStr);

void exitFunc();

void exec(char *command[], history *hisComm[100], int hisCommIter, int backFlag);

int main() {
    // initialization
    char prompt[3] = "> ";
    char input[100];
    char dummy;
    char *command;
    char *temp;
    char *comArr[100];
    int i;
    int backFlag = 0; // 0 means foregroung, 1 means background
    history *hisComm[100]; // INPUT of all commands by client
    int hisCommIter = 0; // number of commands counter

    // initialization of directory struct
    specialdir *dirStr = (specialdir *) malloc(sizeof(specialdir));;
    chdir(getenv("HOME"));
    getcwd(dirStr->homeDir, sizeof(dirStr->homeDir));
    strcpy(dirStr->lastDir, dirStr->homeDir);
    strcpy(dirStr->currDir, dirStr->homeDir);


    // running the shell in the while loop
    while (true) {
        backFlag = 0;
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
                backFlag = 1; // background
                continue;
            }
            if (temp[0] == '"' && temp[strlen(temp)-1] == '"') {
                temp[strlen(temp) - 1] = 0;
                comArr[i] = temp+1;
            } else if (temp[0] == '"') {
                comArr[i] = temp+1;
            } else if (temp[strlen(temp)-1] == '"') {
                temp[strlen(temp) - 1] = 0;
                comArr[i] = temp;
            } else {
                comArr[i] = temp;
            }
            i++;
            temp = strtok(NULL, " ");
        }
        comArr[i] = NULL;
        i--;
        if (!strcmp(comArr[0], "jobs")) {
            // jobs func
            printHisJob(hisComm, hisCommIter, backFlag, 1);
        } else if (!strcmp(comArr[0], "history")) {
            // history func
            printHisJob(hisComm, hisCommIter, backFlag, 0);
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
            exec(comArr, hisComm, hisCommIter, backFlag);
        }
        hisCommIter++;
    }
    return 0;
}

void printHisJob(history *hisComm[100], int hisCommIter, int backFlag, int hisJobFlag) {
    int i = 0;
    pid_t pid;
    int status;
    int jobsFlag = 0;
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error in system call");
        printf("\n");
    } else if (pid == 0) { // child
        for (i = 0; i <= hisCommIter; i++) {
            int tempPid = hisComm[i]->pid;
            if (i == hisCommIter) {
                hisComm[i]->pid = (int) getpid();
                hisComm[i]->status = "RUNNING";
                jobsFlag = 1;
            }
            if (hisComm[i]->status != NULL) {
                if (!kill(tempPid, 0)) {
                    hisComm[i]->status = "RUNNING";
                    jobsFlag = 1;

                } else {
                    hisComm[i]->status = "DONE";
                    jobsFlag = 0;

                }
            } else {
                if (!kill(tempPid, 0)) {
                    hisComm[i]->status = "RUNNING";
                    jobsFlag = 1;
                } else {
                    hisComm[i]->status = "DONE";
                    jobsFlag = 0;
                }
            }
            if (hisJobFlag == 1) { //if we're in Jobs mode
                if (jobsFlag == 1) { // if the command is really in running state
                    if (i < hisCommIter) {
                        printf("%d %s\n", hisComm[i]->pid, hisComm[i]->command);
                    }
                }
            } else { // if we're in HIstory mode
                printf("%d %s %s\n", hisComm[i]->pid, hisComm[i]->command, hisComm[i]->status);
            }
        }
        hisComm[hisCommIter]->status = "DONE";
        exit(0);
    } else {
        hisComm[hisCommIter]->pid = (int) pid;
        signal(SIGCHLD, SIG_IGN);
        wait(&status);
    }
}

void cd(char *command[100], int iter, specialdir *dirStr) {
    int temp = 0;
    char tempDir[1024]; // save last in case cd didn't work
    if (iter >= 2) {
        printf("%d\n", (int) getpid());
        fprintf(stderr, "Error: Too many arguments");
        printf("\n");

    } else {
        char delim = command[iter][0];
        int size = strlen(command[iter]);
        char subbuff[1024];
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
            if (delim == '~') {
                memcpy(subbuff, &command[iter][1], size - 1);
                subbuff[size - 1] = 0;
                chdir(dirStr->homeDir);
                getcwd(dirStr->currDir, sizeof(dirStr->currDir));
                strcat(dirStr->currDir, subbuff);
                dirStr->currDir[strlen(dirStr->homeDir) + strlen(subbuff)] = 0;
                temp = chdir(dirStr->currDir);
                strcpy(dirStr->currDir, command[iter]);
            } else if (delim == '-') {
                memcpy(subbuff, &command[iter][1], size - 1);
                subbuff[size - 1] = 0;
                chdir(tempDir);
                getcwd(dirStr->currDir, sizeof(dirStr->currDir));
                strcat(dirStr->currDir, subbuff);
                dirStr->currDir[strlen(tempDir) + strlen(subbuff)] = 0;
                temp = chdir(dirStr->currDir);
                strcpy(dirStr->currDir, command[iter]);
            } else {
                temp = chdir(command[iter]);
                strcpy(dirStr->currDir, command[iter]);
            }
        }
    }
    if (temp == -1) {
        strcpy(dirStr->lastDir, tempDir);
        strcpy(dirStr->currDir, dirStr->lastDir); // in case cd didn't work go back to last dir
        fprintf(stderr, "Error in system call");
        printf("\n");
    }
}

void exitFunc() {
    printf("%d\n", (int) getpid());
    exit(0);
}

void exec(char *command[100], history *hisComm[100], int hisCommIter, int backFlag) {
    //‪execv‬‬
    int status;
    pid_t pid;
    if ((pid = fork()) < 0) { // error
        fprintf(stderr, "Error in system call");
        printf("\n");
    } else if (pid == 0) { //child
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%d\n", (int) getpid());
        execvp(command[0], command);
        exit(0);
    } else { // father
        hisComm[hisCommIter]->pid = (int) pid;
        signal(SIGCHLD, SIG_IGN);
        if (!backFlag) {
            wait(&status);
        }
    }
}
