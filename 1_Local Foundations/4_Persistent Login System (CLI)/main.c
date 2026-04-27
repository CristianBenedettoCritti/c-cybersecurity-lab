#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct User {
    char username[50];
    char password[50];
};

void simpleHash(char *password);
int registerUser();
int loginUser();

int main(void) {
    char input[3];

    printf("[1] Registration\n[2] Login\n\nOption: ");
    if(fgets(input, 3, stdin) == NULL) {
        perror("fgets input failed");
    }

    if (input[0] == '1') {
        system("cls");
        registerUser();
    }

    if (input[0] == '2') {
        system("cls");
        loginUser();
    }

}

void simpleHash(char *password) {
    char key = 'K'; // Der geheime Schlüssel
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] ^ key; // XOR Operation
    }
}

int registerUser() {
    struct User newUser;
    FILE *fp;

    printf("Register Username: ");
    if(fgets(newUser.username, 50, stdin) == NULL) {
        perror("fgets failed for username");
    }
    newUser.username[strcspn(newUser.username, "\n")] = '\0';

    printf("Register Password: ");
    if(fgets(newUser.password, 50, stdin) == NULL) {
        perror("fgets failed for password");
    }
    newUser.password[strcspn(newUser.password, "\n")] = '\0';

    simpleHash(newUser.password);

    fp = fopen("user.txt", "a");
    if (fp == NULL) {
        perror("File error");
        return 1;
    }

    fprintf(fp, "%s %s\n", newUser.username, newUser.password);
    // fprintf ist dazu da um etwas an einem bestimmten Ort zu printen
    printf("Registration succesful\n");
    fclose(fp);

    return 0;
} 

int loginUser() {
    struct User fileUser;
    char enteredName[50];
    char enteredPass[50];
    FILE *fp;
    int success = 0;

    printf("Username: ");
    if(fgets(enteredName, 50, stdin) == NULL) {
        perror("fgets enteredName failed");
    }
    enteredName[strcspn(enteredName, "\n")] = '\0';

    printf("Password: ");
    if(fgets(enteredPass, 50, stdin) == NULL) {
        perror("fgets enteredPass failed");
    }
    enteredPass[strcspn(enteredPass, "\n")] = '\0';

    simpleHash(enteredPass);

    fp = fopen("user.txt", "r");
    if (fp == NULL) {
        perror("File error");
        return 1;
    }

    while (fscanf(fp, "%s %s", fileUser.username, fileUser.password) != EOF) {
        if (strcmp(enteredName, fileUser.username) == 0 &&
        //  strcmp = "string compare", it compares two strings character by character
        //  returns:
        //  0 => strings are equal
        //  < 0 => first string is smaller
        //  > 0 => first string is greater
            strcmp(enteredPass, fileUser.password) == 0) {
            success = 1;
            break;
            }
    }

    if (success) {
        printf("Login successful :)\n");
    } else {
        printf("Login wasn't succesful :(\n");
    }
}