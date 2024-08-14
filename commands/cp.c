#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

char *filename(char *path);
char *copyFilename(char *srcPath, char *dstPath, bool *mallocatedDP);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s [OPTION] <source> <destination>\n", argv[0]);
        return 1;
    }

    char *srcPath = argv[1];
    char *dstPath = argv[2];
    // if dstPath mallocated or not
    bool mallocatedDP = false;
    // if dstPath exists or not
    bool dstExists = true;
    
    struct stat srcStat;
    struct stat dstStat;
    if (stat(srcPath, &srcStat) != 0) {
        printf("%s: %s: No such file or directory.\n", argv[0], srcPath);
        return 2;
    }
    if (stat(dstPath, &dstStat) != 0) dstExists = false;
    if (!S_ISDIR(srcStat.st_mode)) { // if source path is not a directory
        FILE *src = fopen(srcPath, "r");
        if (src == NULL) {
            printf("%s: %s: No such file or directory.\n", argv[0], srcPath);
            return 2;
        }
        if (dstExists) {
            if (S_ISDIR(dstStat.st_mode)) // if destination path is a directory add filename from srcPath to dstPath
               dstPath = copyFilename(srcPath, dstPath, &mallocatedDP);
        } else {
            char *dstFName = filename(dstPath);
            if (dstFName == NULL) {
                printf("an error occured\n");
                return 3;
            }
            int k = -1;
            for (int i = 1; i < strlen(dstFName) - 1; i++) {
                if (dstFName[i] == '.') {
                    k = i;
                    break;
                }
            }
            if (k == -1) { // if dstPath is a directory
                mkdir(dstPath, 0700);
                dstPath = copyFilename(srcPath, dstPath, &mallocatedDP);
            }
        }

        // check if files are the same by comparing metadata and storage device info
        stat(dstPath, &dstStat);
        if (dstStat.st_ino == srcStat.st_ino && dstStat.st_dev == srcStat.st_dev) {
            printf("%s: %s and %s are the same file.\n", argv[0], dstPath, srcPath);
            return 4;
        }

        FILE *dst = fopen(dstPath, "w");
        if (dst == NULL) {
            printf("%s\n", dstPath);
            printf("%s: %s: No such file or directory.\n", argv[0], dstPath);
            return 2;
        }

        for (char c = getc(src); c != EOF; c = getc(src)) {
            putc(c, dst);
        }

        fclose(src);
        fclose(dst);
        if (mallocatedDP) free(dstPath);
    } else {
        printf("%s: %s is a directory (not copied)\n", argv[0], srcPath);
        return 5;
    }

    return 0;
}

char *filename(char *path) {
    int n = strlen(path);
    if (n <= 0) return NULL;

    int k = -1;
    for(int i = n-1; i >= 0; i--) {
        if (path[i] == '/') {
            k = i;
            break;
        }
    }
    if (k == -1){
        char *fname = malloc(sizeof(char) * (n+1));
        if (strcpy(fname, path) == NULL)
            return NULL;
        return fname;
    }
    
    char *fname = malloc(sizeof(char) * (n - k + 1));

    for (int i = k; i < n; i++) 
        fname[i-k] = path[i];
    fname[n-k] = '\0';

    return fname;
}

char *copyFilename(char *srcPath, char *dstPath, bool *mallocatedDP) {
    char *srcFName = filename(srcPath);
    if (srcFName == NULL) {
        printf("an error occured\n");
        exit(3);
    }

    char *newPath = malloc(sizeof(char) * (strlen(dstPath) + strlen(srcFName) + 1));
    if (newPath == NULL) {
        printf("an error occured\n");
        exit(3);
    }
    *mallocatedDP = true;
    if (strcpy(newPath, dstPath) == NULL) { // copy destination directory to newPath
        printf("an error occured\n");
        exit(3);
    }
    newPath[strlen(dstPath)] = '/';

    if (strcat(newPath, srcFName) == NULL) { // copy destination filename to newPath
        printf("an error occured\n");
        exit(3);
    }
    free(srcFName);
    return newPath;
}
