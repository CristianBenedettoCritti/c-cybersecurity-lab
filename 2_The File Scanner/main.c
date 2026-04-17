#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 101

int analyzeFile(char *fileName);

int main(void){
    char *fileName = malloc(SIZE);

    if(!fileName) {
        perror("malloc failed!");
        return EXIT_FAILURE;
    }

    printf("Enter File Name: ");

    if(fgets(fileName, SIZE, stdin) == NULL) {
        perror("fgets failed!");
        free(fileName);
        return EXIT_FAILURE;
    }
    fileName[strcspn(fileName, "\n")] = '\0';

    analyzeFile(fileName);
    free(fileName);
    return EXIT_SUCCESS;
}

int analyzeFile(char *fileName) {
    char line[256];
    char *keywords[] = {"API_KEY", "password", "token"};
    int lineCounter = 1;
    FILE *ftpr = fopen(fileName, "r");
    if(ftpr == NULL) {
        perror("Error opening file");
        return -1;
    }
    
    while (fgets(line, sizeof(line), ftpr)) {
        // fgets(buffer, size, file_pointer)
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
            if(strstr(line, keywords[i]) != NULL) {
                printf("\nFound secret on line %d: %s", lineCounter, line);
            }
        }
        lineCounter++;
    }

    fclose(ftpr);
}