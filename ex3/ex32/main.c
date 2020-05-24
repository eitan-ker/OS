#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

void readArgFile(int dirFile, char **pathToDir, char **pathToInput, char **pathToOutput);

char* copyLine(char buffer[150],int j, int tabsCounter);

int main(int argc, char* argv[]) {
    char* pathToDir;
    char* pathToInput;
    char* pathToOutput;
    int dirFile = open(argv[1], O_RDONLY);
    if (dirFile == -1) {
        fprintf(stderr, "could not open file");
        return -1;
    }
    // get path from first line - doesn't have to be only only c files, search by .c
    // also need to check if it's dir and if the path is correct
    // path to input file - might not be in the system
    // path to output file - might not be in the system
    readArgFile(dirFile, &pathToDir, &pathToInput, &pathToOutput);

    // open paths - test opening failed
    if (access(pathToDir,F_OK) == -1) {
        fprintf(stderr, "Not a valid directory");
        return -1;
    }
    if (access(pathToInput,F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist");
        return -1;
    }
    if (access(pathToOutput,F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist");
        return -1;
    }


    return 0;
}

void readArgFile(int dirFile, char *pathToDir[150], char *pathToInput[150], char *pathToOutput[150]) {
    char buffer[450];
    int fileReader = 0, tabsCounter = 0, lineNumber = 0, j = 0;
    fileReader = read(dirFile, buffer, 150);
    while (tabsCounter < fileReader) {
        if (buffer[tabsCounter] == '\n') {
            if (lineNumber == 0) {
                *pathToDir = copyLine(buffer, j, tabsCounter);
            } else if (lineNumber == 1) {
                *pathToInput = copyLine(buffer, j, tabsCounter);
            } else if (lineNumber == 2) {
                *pathToOutput = copyLine(buffer, j, tabsCounter);
            }
            lineNumber ++;
            j = tabsCounter + 1;
        }
        tabsCounter++;
    }
}

char* copyLine(char *buffer,int j, int tabsCounter) {

    int i = 0;
    int numOfTabs = tabsCounter - j;
    char* lineToReturn = (char*)malloc(sizeof(char) * (tabsCounter - j + 1));
    for (i = 0; i < numOfTabs; i++) {
        lineToReturn[i] = buffer[j + i];
    }
    lineToReturn[i] = '\0';
    return lineToReturn;
}
