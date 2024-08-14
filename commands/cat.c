#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc <2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    bool showLineNumbers = false;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                if (arg[j] == 'n') { showLineNumbers = true; }
            }
        }
    }

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        int prevChar = ' ';
        int n = 0;
        if (arg[0] != '-') {
            FILE *file = fopen(arg, "r");
            if (file == NULL) {
                printf("%s: %s: No such file or directory\n", argv[0], arg);
                return 1;
            }

            if (showLineNumbers) { printf("%6d\t", ++n); }
            for (int c = getc(file); c != EOF; c = getc(file)) {
                if (showLineNumbers && prevChar == '\n') {
                    printf("%6d\t", ++n);
                }
                putchar(c);
                prevChar = c;
            }
            fclose(file);
        }
    }

    return 0;
}