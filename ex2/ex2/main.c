#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int cd(char *command[], int iter);

int main() {
    // initialization
    char prompt[3] = "> ";
    char input[100];
    char dummy;
    char *command;
    char *temp;
    char *comArr[100];
    int i;

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
            cd(comArr, i);
        } else if (!strcmp(comArr[0], "exit")) {
            // exit func
        } else {
            // exec func
        }
    }
    return 0;
}

int cd(char *command[], int iter) {
    int temp = 0;
    printf("%d\n",(int)getpid());

    //             currentHistory[numOfCommand]->pid = (int)getpid();

    if (iter >= 2) {
        printf("%d\n", (int) getpid());
        fprintf(stderr, "Error: Too many arguments");
    } else {
        char *directory = command[iter];

        temp = chdir(directory);

    }
    if (temp == -1) {
        printf("%d\n", (int) getpid());
        fprintf(stderr, "Error in system call");
    }
}