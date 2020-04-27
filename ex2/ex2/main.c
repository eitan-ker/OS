#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>


int main() {
    char prompt[3] = "> ";
    char input[100], dummy;
    char* command, temp;
    int i =0;
    // running the shell
    while (true) {
        printf("%s", prompt);
        scanf("%[^\n]s",input);
        scanf("%c" ,&dummy);
        command = strtok(input, " ");
        temp = strtok(NULL, " ");

    }
    return 0;
}