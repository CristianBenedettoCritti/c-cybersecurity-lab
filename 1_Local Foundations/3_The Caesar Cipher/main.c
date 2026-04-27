#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE 101 // Kein Semikolon hier!

// Wir geben dem Funktion den Key als Zahl mit
void encryptString(char *string, int key);

int main(void) {
    char *stringToEncrypt = malloc(SIZE);
    char keyBuffer[10]; // Ein kleiner Puffer für die Eingabe der Zahl
    int key;

    if (!stringToEncrypt) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    printf("Enter Phrase to encrypt: ");
    if (fgets(stringToEncrypt, SIZE, stdin) == NULL) {
        free(stringToEncrypt);
        return EXIT_FAILURE;
    }
    stringToEncrypt[strcspn(stringToEncrypt, "\n")] = '\0';

    printf("Enter Key (number): ");
    if (fgets(keyBuffer, sizeof(keyBuffer), stdin) != NULL) {
        key = atoi(keyBuffer); // Wandelt Text "3" in Zahl 3 um
    }

    encryptString(stringToEncrypt, key);

    printf("Encrypted: %s\n", stringToEncrypt);

    free(stringToEncrypt);
    return EXIT_SUCCESS;
}

void encryptString(char *string, int key) {
    // Falls der Key negativ wird er in den positiven Bereich 0-25 gemacht.
    key = (key % 26 + 26) % 26; 

    for (int i = 0; string[i] != '\0'; i++) {
        char c = string[i];

        if (isupper(c)) {
            string[i] = ((c - 'A' + key) % 26) + 'A';
        } else if (islower(c)) {
            string[i] = ((c - 'a' + key) % 26) + 'a';
        }
        // Alles was kein Buchstabe ist (Leerzeichen, !, ?), wird einfach übersprungen.
    }
}