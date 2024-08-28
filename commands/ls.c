#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

char *join(char *base, char *ent);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    bool all = false;
    bool longFormat = false;
    bool color = false;
    char *dirPath = ".";
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "--color") == 0) {
            color = true;
            continue;
        }

        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                switch (arg[j])
                {
                case 'l':
                    longFormat = true;
                    break;
                case 'a':
                    all = true;
                    break;
                default:
                    printf("unknown flag %c\n", arg[j]);
                    break;
                }
            }
        } else {
            dirPath = arg;
        }
    }

    DIR *dirp = opendir(dirPath);
    struct dirent *ent;
    for (ent = readdir(dirp); ent != NULL; ent = readdir(dirp)) {
        if (!all && ent->d_name[0] == '.')
            continue;

        if (color) {
            if (ent->d_type == DT_DIR) 
                printf("\e[34m");
            else if (ent->d_type == DT_REG) {
                struct stat *fstat;
                char *path = join(dirPath, ent->d_name);
                stat(path, fstat);
                if (fstat->st_mode & S_IXUSR) // if file is executable
                    printf("\e[31m");
                free(path);
            }
        }
        printf("%s",ent->d_name);
        if (color) printf("\e[0m");

        printf("\n");
    }
    closedir(dirp);
    printf("\n");

    return 0;
}

char *join(char *base, char *ent) {
    int n = strlen(base);

    char *sep = "";
    if (base[n-1] != '/' && ent[0] != '/') {
        sep = "/";
        n++;
    }

    n += strlen(ent);
    n++; // for \0


    char *path = malloc(sizeof(char)*n);
    
    strcat(path, base);
    strcat(path, sep);
    strcat(path, ent);

    return path;
}