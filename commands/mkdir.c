#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>


int mod2Int(char *mode);
char *filename(char *path);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: mkdir [-pv] [-m mode] directory_name ...\n");
        return 1;
    }
    
    bool intermediate_dirs = false;
    bool verbose = false;
    bool mode_specified = false;
    int mode = -1;

    int i;
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                if (arg[j] == 'p') {
                    intermediate_dirs = true;
                } else if (arg[j] == 'v') {
                    verbose = true;
                } else if (arg[j] == 'm') {
                    mode_specified = true;
                    i++;
                    if (i >= argc){ // Check if mode is specified
                        printf("Mode not specified\n");
                        return 2;
                    }
                    arg = argv[i];
                    if (strlen(arg) != 4) { // Check if mode is 4 characters long
                        printf("Invalid mode: %s\n", arg);
                        return 2;
                    }
                    mode = mod2Int(arg); // Convert mode to integer
                    if (mode == -1) {
                        printf("Invalid mode: %s\n", arg);
                        return 2;
                    }
                    break;
                } else {
                    printf("Invalid option: ");
                    putc(arg[j], stdout);
                    putc('\n', stdout);
                    return 1;
                }
            }
        } else {
            break;
        }
    }

    // if no arguments are given for path
    if (i >= argc) {
        printf("Usage: mkdir [-pv] [-m mode] directory_name ...\n");
        return 1;
    }

    for (; i < argc; i++) {
        char *arg = argv[i];

        char *path = malloc(sizeof(char) * (strlen(arg) + 1));
        if (path == NULL) {
            printf("internal error\n");
            return 3;
        }
        if (arg[0] == '/') {
            path[0] = '/';
            path[1] = '\0';
        }

        char *argcpy = malloc(sizeof(char) * (strlen(arg) + 1));
        if (argcpy == NULL) {
            printf("internal error\n");
            return 3;
        }
        if (strcat(argcpy, arg) == NULL) {
            printf("internal error\n");
            return 3;
        }
        char *leaf = filename(argcpy);
        if (leaf == NULL) {
            printf("internal error\n");
            return 3;
        }
        
        struct stat pathStat;
        for (char *entry = strtok(arg, "/"); entry != NULL; entry = strtok(NULL, "/")) {
            strcat(path, entry);
            strcat(path, "/");
            bool dirExist = stat(path, &pathStat) == 0;
            // if entry is a middle directory
            if (strcmp(entry, leaf) != 0) {
                if (!dirExist && !intermediate_dirs) { 
                    printf("mkdir: %s does not exist\n", path);
                    return 4;
                } else if (dirExist && !S_ISDIR(pathStat.st_mode)) {
                    printf("mkdir: %s is not a directory\n", path);
                    return 5;
                } else if (dirExist && S_ISDIR(pathStat.st_mode)) {
                    continue;
                } else{
                    if (mkdir(path, mode) != 0) {
                        printf("mkdir: %s: %s\n", path, strerror(errno));
                        return 6;
                    }
                    if (verbose) {
                        printf("%s\n", path);
                    }
                }
            } else { // if entry is a leaf directory
                if (dirExist) {
                    printf("mkdir: %s already exists\n", path);
                    return 6;
                }
                if (mkdir(path, mode) != 0) {
                    printf("mkdir: %s: %s\n", path, strerror(errno));
                    return 6;
                }
                if (verbose) {
                    printf("%s\n", path);
                }
            }
            
        }
        free(path);
        free(argcpy);
    }


    return 0;
}


int mod2Int(char *mode) {
    int result = 0;
    if (mode[0] != '0') return -1;
    if (mode[1] < '0' || mode[1] > '7') return -1;
    result += (mode[1] - '0') * 64;
    if (mode[2] < '0' || mode[2] > '7') return -1;
    result += (mode[2] - '0') * 8;
    if (mode[3] < '0' || mode[3] > '7') return -1;
    result += (mode[3] - '0');
    return result;
}

char *filename(char *path) {
    char *filename;
    char *entry = strtok(path, "/");
    while (entry != NULL) {
        filename = entry;
        entry = strtok(NULL, "/");
    }
    return filename;
}
