#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>


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
    }
    return 0;
}