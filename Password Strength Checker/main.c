#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define SIZE 101

char *analyzePassword(char *password);

int main(void) {
    char *password = malloc(SIZE);
    // malloc = memory allocation | password is a pointer 
    // that stores the address of a location in memory.
    // password is the adress and *password points to the things in the adress

    printf("Enter password: ");

    if (!password) {
        perror("malloc failed"); // prints system error
        return EXIT_FAILURE;
    }

    if (fgets(password, SIZE, stdin) == NULL){
        // fgets = “Read user input and put it into memory safely.”

        // Why not scanf?
        // stops at first whitespace → "hello world" → only "hello"
        // no built-in length limit → can overflow your buffer

        // Why fgets?
        // reads whole line (including spaces)
        // limits how much is read → prevents overflow
        // always null-terminates ('\0')
        // It only guarantees '\0' if it succeeds

        // stdin = standard input (keyboard) | stdin = “read from terminal”
        // tells the function where to read from

        // SIZE tells fgets "Never read more than this many characters - 1"
        perror("fgets failed");
        free(password);
        return EXIT_FAILURE;
    }
    password[strcspn(password, "\n")] = '\0';
    char *result = analyzePassword(password);
    printf("%s\n", result);
    free(password);
    return EXIT_SUCCESS;
}

char *analyzePassword(char *password) {
    int len = 0;
    int hasUpper = 0;
    int hasLower = 0;
    int hasUpLow = 0;
    int hasDigit = 0;
    int hasSpecialCharacter = 0;
    int pointResult = 0;

    if(password)

    for (int i = 0; password[i] != '\0'; i++){
        if(!isspace(password[i])) {
            len++;
        }

        if(islower(password[i])) {
            hasLower = 1;
        }
        
        if(isupper(password[i])) {
            hasUpper = 1;
        }

        if(isdigit(password[i])) {
            hasDigit = 1;
        }

        if(ispunct(password[i])) {
            hasSpecialCharacter = 1;
        }
    }

    if(hasUpper && hasLower) {
        hasUpLow = 1;
    }

    if(len < 8) {
        return "Too short!";
    }

    pointResult = hasUpLow + hasDigit + hasSpecialCharacter;
    if (pointResult <= 1) {
        return "Weak password";
    } else if (pointResult == 2) {
        return "Medium password";
    } else if (pointResult == 3) {
        return "Strong password";
    }
    return "Something went wrong";
};