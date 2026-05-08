#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

char username[32];
char xor_key = 'X';

DWORD WINAPI receive_messages(LPVOID lpParam);
void xor_cipher(char *data, char key);

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server_config;
    char buffer[1024];
    char formatted_buffer[1100];

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);

    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_config.sin_port = htons(8888);

//     connect(socket, address pointer, size)
    if(connect(s, (struct sockaddr *)&server_config, sizeof(server_config)) < 0) {
        printf("Connection failed! Is the server running?\n");
        return 1;
    }

    // Let the server know who you are
    send(s, username, strlen(username), 0);

    system("cls");
    printf("Connected to Chat! Type your message belowr:\n");

    CreateThread(NULL, 0, receive_messages, (LPVOID)s, 0, NULL);

    while(1) {
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);

        // encrypt before the message gets send
        xor_cipher(buffer, xor_key);

        // Build the packet: "Name: [encrypted message]"
        sprintf(formatted_buffer, "%s: %s", username, buffer);

        send(s, formatted_buffer, strlen(formatted_buffer), 0);
    }

    closesocket(s);
    WSACleanup();
    return 0;
}

DWORD WINAPI receive_messages(LPVOID lpParam) {
    SOCKET s = (SOCKET)lpParam;
    char buffer[1200];
    int bytes;

    while(1) {
        bytes = recv(s, buffer, sizeof(buffer) -1, 0);
        if(bytes > 0) {
            buffer[bytes] = '\0';

            char *message_part = strchr(buffer, ':');
            if(message_part != NULL) {
            //  we go 2 char after the ':'
                message_part += 2;

                xor_cipher(message_part, xor_key);
            }

            printf("\r%s ", buffer);
            printf("                                \r");
            printf("You: ");
            fflush(stdout);
        } else {
            printf("\nLost connection to server.\n");
            exit(1);
        }
    }
    return 0;
}

void xor_cipher(char *data, char key) {
    for(int i = 0; i < strlen(data); i++) {
        // only encrpyt if there isnt a \r
        if(data[i] != '\n' && data[i] != '\r') {
            data[i] = data[i] ^ key;
        }
    }
}