#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 101

int lenCheck(char password[]);
int UpLowCaseCheck(char password[]);
int numberCheck(char password[]);
int specialCharacterCheck(char password[]);
char *getResult(int a, int b, int c, int d);

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
    int lenResult = lenCheck(password);
    int upLowCaseResult = UpLowCaseCheck(password);
    int numberResult = numberCheck(password);
    int specialCharacterResult = specialCharacterCheck(password);
    char *finalResult = getResult(lenResult, upLowCaseResult, numberResult, specialCharacterResult);
    printf("%s\n", finalResult);
    free(password);
    return EXIT_SUCCESS;
}

int lenCheck(char password[]) {
    int len = strlen(password);

    if (len < 8){
        return 0;
    }

    return 1;
}

int UpLowCaseCheck(char password[]) {
    int hasLower = 0;
    int hasUpper = 0;

    for (int i = 0; password[i] != '\0'; i++) {
        if(islower(password[i])) {
            hasLower = 1;
        }

        if(isupper(password[i])) {
            hasUpper = 1;
        }

        if (hasLower && hasUpper) {
            return 1;
        }
    }
    return 0;
}


int numberCheck(char password[]) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (isdigit(password[i])) {
                return 1;
            }
    }
    return 0;
}

int specialCharacterCheck(char password[]) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (ispunct(password[i])) {
            return 1;
        }
    }
    return 0;
}

char *getResult(int a, int b, int c, int d) {
    int pointResult = a + b + c + d;

    if (pointResult <= 2) {
        return "Weak password";
    } else if (pointResult == 3) {
        return "Medium password";
    } else if (pointResult == 4) {
        return "Strong password";
    }
    return "Something went wrong";
}