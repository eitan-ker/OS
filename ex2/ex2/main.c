#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int cd (char* command[], int iter);

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
        // inserting all flags
        while (temp != NULL) {
            comArr[i] = temp;
            i++;
            temp = strtok(NULL, " ");
        }
        comArr[i] = NULL;
        if (!strcmp(comArr[0], "jobs")) {
            printf("jobs\n");
            // jobs func
        } else if (!strcmp(comArr[0], "history")) {
            printf("history\n");
            // history func
        } else if (!strcmp(comArr[0], "cd")) {
            printf("cd\n");
            // cd func
            cd(comArr, i);
        } else if (!strcmp(comArr[0], "exit")) {
            printf("exit\n");
            // exit func
        } else {
            printf("else\n");
            // exec func
        }
    }
    return 0;
}

int cd (char* command[], int iter) {

}