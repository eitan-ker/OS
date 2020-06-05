#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <wait.h>
#include <time.h>

void readArgFile(int dirFile, char **pathToDir, char **pathToInput, char **pathToOutput);

char *copyLine(char buffer[150], int j, int tabsCounter);

void startTest(char *pathToDir, char *pathToInput, char *pathToOutput);

int searchCFilesReturnScore(DIR *insideHomeDir, char *pathToInput,
                            char *pathToOutput, char *insideFirstDir);

int compileAndScore(char *pathToInput, char *pathToOutput, struct dirent *inDir,
                    char *fileInsideDIr, char *file);

int compile(char *fileInsideDIr, char *fileTxtOutput, char *pathToInput, char *pathToOutput);

int getGrade(int flag);

int runCompiled(char *fileTxtOutput, char *pathToInput, char *pathToOutput);

int TestComp(char *fileTxtOutput, char *pathToOutput);

void printScore(int grade, char *dir, int resultFile);

int main(int argc, char *argv[]) {
    char *pathToDir;
    char *pathToInput;
    char *pathToOutput;
    int dirFile = open(argv[1], O_RDONLY);
    if (dirFile == -1) {
        fprintf(stderr, "could not open file\n");
        return -1;
    }
    // get path from first line - doesn't have to be only only c files, search by .c
    // also need to check if it's dir and if the path is correct
    // path to input file - might not be in the system
    // path to output file - might not be in the system
    readArgFile(dirFile, &pathToDir, &pathToInput, &pathToOutput);

    // open paths - test opening failed
    if (access(pathToDir, F_OK) == -1) {
        fprintf(stderr, "Not a valid directory\n");
        return -1;
    }
    if (access(pathToInput, F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist\n");
        return -1;
    }
    if (access(pathToOutput, F_OK) == -1) {
        fprintf(stderr, "Input/output File not exist\n");
        return -1;
    }

    startTest(pathToDir, pathToInput, pathToOutput);


    return 0;
}

void startTest(char *pathToDir, char *pathToInput, char *pathToOutput) {
    struct dirent *firstDir;
    struct stat fileStat;
    DIR *homeDir = opendir(pathToDir);
    DIR *insideHomeDir;
    int grade = 0;
    if (!homeDir) {
        fprintf(stderr, "error in system call\n");
        exit(1);
    }
    int resultFile = open("result.csv", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
    if (resultFile == -1) {
        fprintf(stderr, "could not open file\n");
        exit(1);
    }
    // for each dir inside the pathToDir it runs the testing for the grade
    while ((firstDir = readdir(homeDir)) != NULL) {
        char *dir = firstDir->d_name;
        if ((strcmp(firstDir->d_name, "..") == 0) || (strcmp(firstDir->d_name, ".") == 0)) {
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
                fprintf(stderr, "could not open file\n");
            }
            insideHomeDir = opendir(fileInsideDir);
            if (!insideHomeDir) {
                fprintf(stderr, "error in system call\n");
                exit(1);
            }
            // search for c files
            grade = searchCFilesReturnScore(insideHomeDir, pathToInput, pathToOutput,
                                            fileInsideDir); //score result from files
            // write score for file
            printScore(grade, dir, resultFile);
        }
    }

}

void printScore(int grade, char *dir, int resultFile) {
    // supposed to be create from the same place the program runs
    char stringToPrint[1024];;
    int length;
    char* sGrade;
    char* wGrade;

    strcpy(stringToPrint, dir);
    strcat(stringToPrint, ",");
    if (grade == 0) {
        sGrade = "0";
        wGrade = "NO_C_FILE";
    } else if (grade == 10) {
        sGrade = "10";
        wGrade = "COMPILATION_ERROR";
    } else if (grade == 20) {
        sGrade = "20";
        wGrade = "TIMEOUT";
    } else if (grade == 50) {
        sGrade = "50";
        wGrade = "WRONG";
    } else if (grade == 75) {
        sGrade = "75";
        wGrade = "SIMILAR";
    } else if (grade == 100) {
        sGrade = "100";
        wGrade = "EXCELLENT";
    }
    strcat(stringToPrint, sGrade);
    strcat(stringToPrint, ",");
    strcat(stringToPrint, wGrade);
    strcat(stringToPrint, "\n");
    length = write(resultFile, stringToPrint, strlen(stringToPrint));
    if (length != strlen(stringToPrint)) {
        fprintf(stderr, "error in system call\n");
        exit(1);
    }
}


int searchCFilesReturnScore(DIR *insideHomeDir, char *pathToInput,
                            char *pathToOutput, char *fileInsideDIr) {
    struct dirent *inDir;
    int fileNameLength = 0;
    int cFileFlag = 0;
    while ((inDir = readdir(insideHomeDir)) != NULL) {
        cFileFlag++;
        char *file = inDir->d_name;
        if ((strcmp(inDir->d_name, "..") == 0) || (strcmp(inDir->d_name, ".") == 0)) {
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

int compileAndScore(char *pathToInput, char *pathToOutput, struct dirent *inDir,
                    char *fileInsideDIr, char *file) {
    char fileTxtOutput[150];
    // make the result file
    strcpy(fileTxtOutput, "./");
    strcat(fileTxtOutput, file);
    strcat(fileTxtOutput, ".txt");
    // the C file
    strcat(fileInsideDIr, "/");
    strcat(fileInsideDIr, file);
    // compile file
    // run compiled file with the input file
    // test output with output file using ex31 func
    // return score
    return compile(fileInsideDIr, fileTxtOutput, pathToInput, pathToOutput);
}

int compile(char *fileInsideDIr, char *fileTxtOutput, char *pathToInput, char *pathToOutput) {
    pid_t pid;
    int status = 0;
    int gradeFlag = 0;
    if ((pid = fork()) == 0) {
        char *argv[3];
        argv[0] = "gcc";
        argv[1] = fileInsideDIr;
        argv[2] = NULL;
        execvp("gcc", argv);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) == 1) {
            gradeFlag = 2; // second condition for compilation error
            return getGrade(gradeFlag);
        }
        // compilation succeeded
        return runCompiled(fileTxtOutput, pathToInput, pathToOutput);
    }
}

int runCompiled(char *fileTxtOutput, char *pathToInput, char *pathToOutput) {
    int input, txtOutput, status = 0;
    int gradeFlag = 0;
    int grade = 0;
    pid_t pid;
    time_t beforeExec, afterExec;
    input = open(fileTxtOutput, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
    txtOutput = open(pathToInput, O_RDONLY);
    if ((input == -1) || (txtOutput == -1)) {
        fprintf(stderr, "error in system call\n");
        exit(1);
    }
    if (pid = fork() == 0) {
        dup2(input, 1);
        dup2(txtOutput, 0);
        close(input);
        close(txtOutput);
        char *argv[2];
        argv[0] = "a.out";
        argv[1] = NULL;
        execvp("./a.out", argv);
        exit(1);
    } else {
        time(&beforeExec);
        waitpid(pid, &status, 0);
        time(&afterExec);
        double deltaTime = afterExec - beforeExec;
        if (deltaTime > 3) {
            gradeFlag = 3;
            return getGrade(gradeFlag);
        }
        grade = TestComp(fileTxtOutput, pathToOutput);
        close(input);
        close(txtOutput);
        return grade;
    }
}

int TestComp(char *fileTxtOutput, char *pathToOutput) {
    pid_t pid;
    int status = 0;
    int gradeFlag = 0;
    if (pid = fork() == 0) {
        char *argv[4];
        argv[0] = "comp.out";
        argv[1] = fileTxtOutput;
        argv[2] = pathToOutput;
        argv[3] = NULL;
        execvp("./comp.out", argv);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        int st = WEXITSTATUS(status);
        if (WEXITSTATUS(status) == 2) {
            gradeFlag = 4;
        } else if (WEXITSTATUS(status) == 3) {
            gradeFlag = 5;
        } else if (WEXITSTATUS(status) == 1) {
            gradeFlag = 6;
        }
        return getGrade(gradeFlag);
    }
}

int getGrade(int gradeFlag) {
    if (gradeFlag == 2) {
        return 10;
    } else if (gradeFlag == 3) {
        return 20;
    } else if (gradeFlag == 4) {
        return 50;
    } else if (gradeFlag == 5) {
        return 75;
    } else if (gradeFlag == 6) {
        return 100;
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
            lineNumber++;
            j = tabsCounter + 1;
        }
        tabsCounter++;
    }
}

char *copyLine(char *buffer, int j, int tabsCounter) {

    int i = 0;
    int numOfTabs = tabsCounter - j;
    char *lineToReturn = (char *) malloc(sizeof(char) * (tabsCounter - j + 1));
    for (i = 0; i < numOfTabs; i++) {
        lineToReturn[i] = buffer[j + i];
    }
    lineToReturn[i] = '\0';
    return lineToReturn;
}
