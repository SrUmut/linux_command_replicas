#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

typedef struct {
    char *name;
    bool a;
} conf;

// a is a list of prefixes
//      1: "    "
//      2: "│   "
// n is the length of the a
typedef struct {
    char *a;
    int n;
} prefix;

void handlePath(conf *cmd, char *path);
char *join(char *base, char *ent);
void printwprf(char *s, prefix prf);
void tree(conf *cmd, char *path, char *name, prefix prf, int depth);
void append(prefix *prf, char c, int depth);
void copy(char dst[], int nd, char src[]);

int main(int argc, char *argv[]) {
    char *path = NULL;
    if (argc < 2) {
        path = ".";
    }
    
    conf cmd;
    cmd.name = argv[0];
    cmd.a = false;
    
    // parse the flag(s)
    int i;
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                switch (arg[j]) {
                case 'a':
                    cmd.a = true;
                    break;
                default:
                    printf("%s: unknown flag: ", argv[0]);
                    putc(arg[j], stdout);
                    putc('\n', stdout);
                    break;
                }
            }
        } else {
            break;
        }
    }

    // get the path(s)
    if (i >= argc) {
        path = ".";
    }
    if (path == NULL) {
        for (; i < argc; i++) {
            path = argv[i];
            handlePath(&cmd, path);
        }
    } else {
        handlePath(&cmd, path);
    }

    return 0;
}

// joins two paths
char *join(char *base, char *ent) {
    int baselen = strlen(base);
    if (base[baselen-1] == '/') {
        base[baselen-1] = '\0'; 
        baselen--;
    }
    int entlen = strlen(ent);
    int n = baselen + entlen + 2;
    char *path = malloc(sizeof(char) * n);
    if (path == NULL) {
        printf("error while allocating memory\n");
        exit(5);
    }
    strcat(path, base);
    strcat(path, "/");
    strcat(path, ent);
    return path;
}

// a recursive function to traverse a directory
void tree(conf *cmd, char *path, char *name, prefix prf, int depth) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("\n%s: %s on path: %s\n", cmd->name, strerror(errno), path);
        exit(4);
    }

    struct dirent *ent;
    struct dirent *nextent;
    struct stat entstat;
    printf("%s\n",name);
    for (ent = readdir(dir), nextent = readdir(dir); ent != NULL; ent = nextent, nextent = readdir(dir)) {
        // if -a flag is not used skip the entries begin with .
        if ((!cmd->a && ent->d_name[0] == '.' && ent->d_type == 4) || 
            (cmd->a && (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0))) {
            continue;
        }

        char *entpath = join(path, ent->d_name);
        // get the stats for the entry
        if (stat(entpath, &entstat) != 0) {
            printf("\n%s: %s\n", path, strerror(errno));
            exit(2);
        }

        // print the prefix and set the prefix for sub elements
        // based on if there is another element in the same level
        if (nextent == NULL) {
            printwprf("└── ", prf);
            append(&prf, 1, depth);
        } else {
            printwprf("├── ", prf);
            append(&prf, 2, depth);
        }

        // print name if it is a file
        // traverse the directory if it is a directory
        if (!S_ISDIR(entstat.st_mode)) {
            printf("%s\n", ent->d_name);
        } else {
            tree(cmd, entpath, ent->d_name, prf, depth+1);
        }
        prf.a[depth] = '\0';
        free(entpath);
    }
    closedir(dir);
}

// print with prefix
void printwprf(char *s, prefix prf) {
    for ( ; *prf.a != '\0'; prf.a++) {
        if (*prf.a == 1) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }
    printf("%s", s);
}

// handles the root path which is given to the command
void handlePath(conf *cmd, char *path) {
    struct stat ds;
    if (stat(path, &ds) != 0) {
        printf("%s: %s: %s\n", cmd->name, path, strerror(errno));
        exit(2);
    }

    if (!S_ISDIR(ds.st_mode)) {
        printf("%s: %s is not a directory\n", cmd->name, path);
        exit(3);
    }

    int n = 16;
    prefix prf;
    prf.a = malloc(sizeof(char)*16);
    prf.n = n;
    for (int i = 0; i < n; i++) {
        prf.a[i] = '\0';
    }
    tree(cmd, path, path, prf, 0);
    printf("\n");
    free(prf.a);
}

// add new element to the prefix
void append(prefix *prf, char c, int depth) {
    if (depth < prf->n) {
        prf->a[depth] = c;
        return;
    }

    char *arr = malloc(sizeof(char) * prf->n * 2);
    copy(arr, prf->n * 2, prf->a);
    arr[depth] = c;
    
    free(prf->a);
    prf->n *= 2;
    prf->a = arr;
}

void copy(char dst[], int nd, char src[]) {
    char *dest = dst;
    while ((*dest++ = *src++));
    for (dest++; dest < dst+nd; dest++) {
        *dest = '\0';
    }
}
