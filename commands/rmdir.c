#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


int locateSep(char *s, int startIdx, char sep);
void delDir(char *path, bool verbose);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: rmdir [-pv] directory ...\n");
        return 1;
    }
    
    bool intermediate_dirs = false;
    bool verbose = false;

    int i;
    // parse the flags
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                switch (arg[j]) {
                case 'p':
                    intermediate_dirs = true;
                    break;
                case 'v':
                    verbose = true;
                    break;
                default:
                    printf("%s: invalid flag: ", argv[0]);
                    putc(arg[j], stdout);
                    putc('\n', stdout);
                    return 2;
                }
            }
        } else break;
    }

    // if no argument is given for path
    if (i >= argc) {
        printf("Usage: rmdir [-pv] directory ...\n");
        return 3;
    }

    for (; i < argc; i++) {
        char *arg = argv[i];

        // if delete intermediate directories flag is not used
        if (!intermediate_dirs) {
            delDir(arg, verbose);
        }
        // if delete intermediate directories flag is used 
        else {
            // delete leaf dir
            delDir(arg, verbose);
            
            // delete intermediate directories
            int sepIdx = locateSep(arg, strlen(arg), '/');
            while (true) {
                if (sepIdx < 0) break;
                
                arg[sepIdx] = '\0';
                delDir(arg, verbose);

                sepIdx = locateSep(arg, strlen(arg), '/');
            }

        }
    }

    return 0;
}

int locateSep(char *s, int startIdx, char sep) {
    if (startIdx <= 0) return -1;

    for (int i = startIdx - 1; i >= 0; i--) {
        if (s[i] == sep) {
            if (i != startIdx-1) return i;
            else startIdx--;
        }
    }

    return -1;
}

void delDir(char *path, bool verbose) {
    struct stat pathStat;
    if (stat(path, &pathStat) != 0) {
        printf("rmdir: %s: %s\n", path, strerror(errno));
        exit(4);
    }
    if (!S_ISDIR(pathStat.st_mode)) {
        printf("rmdir: %s is not a directory\n", path);
        exit(5);
    }
    if (rmdir(path) != 0) {
        printf("rmdir: %s: %s\n", path, strerror(errno));
        exit(6);
    } else if (verbose) printf("%s\n", path);
}