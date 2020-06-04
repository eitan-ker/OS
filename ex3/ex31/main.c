#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int checkFiles(char *file1, char *file2, int file1_size, int file2_size);

int checkCorrespondence(char buff1[1024], char buff2[1024], int size1, int size2);

int min(int size1, int size2);

int max(int size1, int size2);


int main(int argc, char *argv[]) {
    struct stat file_1_stat, file_2_stat;
    char *path1 = argv[1];
    char *path2 = argv[2];
    int file1_size = 0, file2_size = 0, ret_value;
    long int file1 = 0, file2 = 0;

    if (argc != 3) {
        return -1;
    }

    // if it is the same path = same file
    if (strcmp(path1, path2) == 0) {
        return 1;
    }

    // open file1
    if ((file1 = open(path1, O_RDONLY)) == -1) {
        fprintf(stderr, "there is an error in stat function in file 1");
        return -1;
    }
    if (stat(path1, &file_1_stat) == -1) {
        return -1;
    }
    file1_size = file_1_stat.st_size;

    // open file2
    if ((file2 = open(path2, O_RDONLY)) == -1) {
        fprintf(stderr, "there is an error in stat function in file 2");
        return -1;
    }
    if (stat(path2, &file_2_stat) == -1) {
        return -1;
    }
    file2_size = file_2_stat.st_size;

    if (file1 == file2) {
        return 1;
    }

    ret_value = checkFiles(file1, file2, file1_size, file2_size);

    close(file1);
    close(file2);

    if (ret_value == -1) {
        return -1;
    }
    return ret_value;
}

// returns the number of matches
int checkCorrespondence(char buff1[1024], char buff2[1024], int size1, int size2) {
    int i, counter = 0, minSize = 0;
    minSize = min(size1, size2); // return min is different size, or one of them if the same size
    for (i = 0; i < minSize; i++) {
        if (buff1[i] == buff2[i]) {
            counter++;
        }
    }
    return counter;
}

// might be identical / similar / different
int checkFiles(char *file1, char *file2, int file1_size, int file2_size) {
    int flag; // locally: 1 - identical, 3 - similar, 2 - different
    int matches = 0, maxMatches = 0, matchesCounter = 0, maxBufSize = 0;
    int i = 0, j = 0, minFileSize, maxFileSize;
    char buff1[1024];
    char buff2[1024];
    int buff1_size = 0;
    int buff2_size = 0;
    int chars_readed = 0;
    minFileSize = min(file1_size, file2_size);
    maxFileSize = max(file1_size, file2_size);
    while (chars_readed < maxFileSize) {

        for (i = 0; i < file1_size; i++) {
            if (buff1_size >= file1_size) {
                break;
            }
            if(lseek(file1, i, SEEK_SET == -1)) {
                fprintf(stderr, "there is an error in lseek function in file 1");
                return -1;
            }
            for (j = 0; j < file2_size; j++) {
                if (buff2_size >= file2_size) {
                    break;
                }
                if (lseek(file2, j, SEEK_SET == -1)) {
                    fprintf(stderr, "there is an error in lseek function in file 2");
                    return -1;
                }
                buff1_size = read(file1, buff1, 1024);
                buff2_size = read(file2, buff2, 1024);
                if (i == 0) {
                    maxBufSize = buff1_size;
                }

                matches = checkCorrespondence(buff1, buff2, buff1_size, buff2_size);
                if (matches >= maxMatches) {
                    maxMatches = matches;
                }
            }
        }
        matchesCounter = matchesCounter + maxMatches;
        chars_readed = chars_readed + maxBufSize;
    }
    if (matchesCounter == maxFileSize) {
        flag = 1;
    } else if (matchesCounter >= ((minFileSize / 2) + 1)) {
        return 3;
    } else {
        return 2;
    }
    return flag;
}

int min(int size1, int size2) {
    if (size1 > size2) {
        return size2;
    } else {
        return size1;
    }
}

int max(int size1, int size2) {
    if (size1 > size2) {
        return size1;
    } else {
        return size2;
    }
}


