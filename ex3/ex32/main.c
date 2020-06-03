#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void readArgFile(int dirFile, char **pathToDir, char **pathToInput, char **pathToOutput);

char* copyLine(char buffer[150],int j, int tabsCounter);

void startTest(char* pathToDir, char *pathToInput, char *pathToOutput);

int searchCFilesReturnScore(DIR* insideHomeDir, char *pathToInput,
                            char *pathToOutput, char* insideFirstDir);
int compileAndScore(char *pathToInput, char *pathToOutput, struct dirent* inDir,
                char* fileInsideDIr, char *file);

void compile();

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
        fprintf(stderr, "Not a valid directory\n");
        return -1;
    }
    if (access(pathToInput,F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist\n");
        return -1;
    }
    if (access(pathToOutput,F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist\n");
        return -1;
    }

    startTest(pathToDir, pathToInput, pathToOutput);


    return 0;
}

void startTest(char* pathToDir, char *pathToInput, char *pathToOutput) {
    struct dirent* firstDir;
    struct stat fileStat;
    DIR* homeDir = opendir(pathToDir);
    DIR* insideHomeDir;
    int score = 0;
    if (!homeDir) {
        fprintf(stderr, "error in system call");
        exit(1);
    }
    int file = open("result.csv", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IWGRP|S_IWOTH);
    if (file == -1) {
        fprintf(stderr, "could not open file\n");
        exit(1);
    }
    while ((firstDir = readdir(homeDir)) != NULL) {
        char * dir = firstDir->d_name;
        if ((strcmp(firstDir->d_name, "..")==0) || (strcmp(firstDir->d_name, ".")==0)) {
            continue;
        }
        if (firstDir->d_type == 4) // if the path inside the first dir is a dir
        {
            char fileInsideDir[150];
            // make path to dir.
            strcpy(fileInsideDir, pathToDir);
            strcat(fileInsideDir, "/");
            strcat(fileInsideDir, firstDir->d_name);
            // open inside Dir
            if (stat(fileInsideDir, &fileStat) == -1) {
                fprintf(stderr, "could not open file");
            }
            insideHomeDir = opendir(fileInsideDir);
            if (!insideHomeDir) {
                fprintf(stderr, "error in system call");
                exit(1);
            }
            // search for c files
            score = searchCFilesReturnScore(insideHomeDir, pathToInput, pathToOutput, fileInsideDir); //score result from files
            int c =2;
            // write score for file
        }
    }

}

int searchCFilesReturnScore(DIR* insideHomeDir, char *pathToInput,
        char *pathToOutput, char* fileInsideDIr) {
    struct dirent* inDir;

    int fileNameLength = 0;
    int cFileFlag = 0;
    while ((inDir = readdir(insideHomeDir)) != NULL) {
        cFileFlag++;
        char *file = inDir->d_name;
        if ((strcmp(inDir->d_name, "..")==0) || (strcmp(inDir->d_name, ".")==0)) {
            continue;
        }
        if (inDir->d_type == 8) // if the file might be c type
        {
            fileNameLength = strlen(inDir->d_name);
            char type = inDir->d_name[fileNameLength - 1];
            char point = inDir->d_name[fileNameLength - 2];
            if (type == 'c' && point == '.') {
                return compileAndScore(pathToInput, pathToOutput, inDir, fileInsideDIr, file);
            } else {
                cFileFlag--;
            }
        } else {
            cFileFlag--;
        }
    }
    if (cFileFlag == 0) { // no C file in dir
        return 0;
    }
}

int compileAndScore(char *pathToInput, char *pathToOutput, struct dirent* inDir,
        char* fileInsideDIr, char *file) {
    char fileExe[150];
    char fileTxtOutput[150];
    // make the exe file
    strcpy(fileExe, fileInsideDIr);
    strcat(fileExe, "/");
    strcat(fileExe, "a.out");
    // make the result file
    strcpy(fileTxtOutput, "./");
    strcat(fileTxtOutput, file);
    strcat(fileTxtOutput, ".txt");
    // the C file
    strcat(fileInsideDIr, "/");
    strcat(fileInsideDIr, file);
    // compile file
    compile(fileInsideDIr, fileExe, fileTxtOutput);


    // run compiled file with the input file

    // test output with output file using ex31 func

    // return score
}

void compile(char* fileInsideDIr, char* fileExe, char* fileTxtOutput) {
    pid_t pid1, pid2,pid3;
    if (pid1 == fork() == 0) {
        char* argv[3];
        argv[0] = "gcc";
        argv[1] = fileInsideDIr;
        argv[2] = NULL
        int d =3;
    }
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
