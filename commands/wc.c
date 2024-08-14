#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fileCount = 0;
    bool printLCount = false;
    bool printWCount = false;
    bool printCCount = false;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        // if argument is a flag
        if (arg[0] == '-') {
            for (int j = 1; j < strlen(arg); j++) {
                switch (arg[j])
                {
                case 'l':
                    printLCount = true;
                    break;
                case 'w':
                    printWCount = true;
                    break;
                case 'c':
                    printCCount = true;
                    break;
                default:
                    break;
                }
            }
        } else {
            fileCount++;
        }
    }

    int totalCharCount = 0;
    int totalWordCount = 0;
    int totalLineCount = 0;
    int charCount;
    int wordCount;
    int lineCount;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {
            FILE *file = fopen(arg, "r");
            if (file == NULL) {
                printf("%s: %s: No such file or directory\n", argv[0], arg);
                return 1;
            }

            charCount = 0;
            wordCount = 0;
            lineCount = 0;

            int prevChar = ' ';
            for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
                charCount++;
                if ((c == ' ' || c == '\n' || c == '\t') && (prevChar != ' ' && prevChar != '\n' && prevChar != '\t')) {
                    wordCount++;
                }
                if (c == '\n') {
                    lineCount++;
                }
                prevChar = c;
            }
            if (prevChar != ' ' && prevChar != '\n' && prevChar != '\t') {
                wordCount++;
            }
            totalCharCount += charCount;
            totalWordCount += wordCount;
            totalLineCount += lineCount;

            if (!(printCCount || printWCount || printLCount)) {
                printf("\t%d\t%d\t%d\t%s\n", lineCount, wordCount, charCount, arg);
            } else {
                if (printLCount) { printf("\t%d", lineCount); }
                if (printWCount) { printf("\t%d", wordCount); }
                if (printCCount) { printf("\t%d", charCount); }
                printf("\t%s\n", arg);
            }
            fclose(file);
        }
    }

    if (fileCount > 1) {
        if (!(printCCount || printWCount || printLCount)) {
            printf("\t%d\t%d\t%d\ttotal\n", totalLineCount, totalWordCount, totalCharCount);
        } else {
            if (printLCount) { printf("\t%d", totalLineCount); }
            if (printWCount) { printf("\t%d", totalWordCount); }
            if (printCCount) { printf("\t%d", totalCharCount); }
            printf("\ttotal\n");
        }
    }

    return 0;
}