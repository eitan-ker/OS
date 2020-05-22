#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int sameSize(char *file1, char *file2, int files_size);

int differentSize(char *file1, char *file2, int size1, int size2);

int checkCorrespondence(char buff1[1024], char buff2[1024], int size1, int size2);

int min(int size1, int size2);


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
    }
    if (stat(path1, &file_1_stat) == -1) {
        return -1;
    }
    file1_size = file_1_stat.st_size;

    // open file2
    if ((file2 = open(path2, O_RDONLY)) == -1) {
        fprintf(stderr, "there is an error in stat function in file 1");
    }
    if (stat(path2, &file_2_stat) == -1) {
        return -1;
    }
    file2_size = file_2_stat.st_size;

    if (file1 == file2) {
        return 1;
    }

    if (file1_size == file2_size) {
        ret_value = sameSize(file1, file2, file1_size);
    } else if (file1_size > file2_size) {
        ret_value = differentSize(file1, file2, file1_size, file2_size);
    } else if (file1_size < file2_size) {
        ret_value = differentSize(file1, file2, file1_size, file2_size);
    }

    close(file1);
    close(file2);

    return ret_value;
}

// returns the number of matches
int checkCorrespondence(char buff1[1024], char buff2[1024], int size1, int size2) {
    int i, counter = 0, minSize = 0;
    if (size1 == size2) { // might be identical
        // identical check
        for (i = 0; i < size1; i++) {
            if (buff1[i] == buff2[i]) {
                counter ++;
            } else {

                // might have הסתים שונים ואז דומים

            }
        }
    } else { // might be similar
        minSize = min(size1, size2);
        for (i = 0; i < minSize; i++) {
            if (buff1[i] == buff2[i]) {
                counter ++;
            }
        }
    }
    return counter;
}

// might be identical / similar / different
int sameSize(char *file1, char *file2, int files_size) {
    int flag; // locally: 1 - identical, 3 - similar, 2 - different
    int matches = 0;
    char buff1[1024];
    char buff2[1024];
    int buff1_size = 0;
    int buff2_size = 0;
    int chars_readed = 0;
    while (chars_readed < files_size) {
        buff1_size = read(file1, buff1, 1024);
        buff2_size = read(file2, buff2, 1024);
        matches = matches + checkCorrespondence(buff1, buff2, buff1_size, buff2_size);
        chars_readed = chars_readed + buff1_size;
    }
    if (matches == files_size) {
        flag = 1;
    } else if (matches >= ((files_size/2) + 1)) {
        return 3;
    } else {
        return 2;
    }
    return flag;
}

// might be similar / different
int differentSize(char *file1, char *file2, int size1, int size2) {

}

int min(int size1, int size2) {
    if (size1 > size2) {
        return size2;
    } else {
        return size1;
    }
}


