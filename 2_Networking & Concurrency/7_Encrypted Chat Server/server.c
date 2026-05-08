#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define MAX_CLIENTS 10

SOCKET clients[MAX_CLIENTS];
int client_count = 0;
HANDLE mutex;

DWORD WINAPI handle_client(LPVOID lpParam);

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_config, client_config;
    int client_config_size = sizeof(client_config);

    mutex = CreateMutex(NULL, FALSE, NULL);
    WSAStartup(MAKEWORD(2, 2), &wsa);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = INADDR_ANY;
    server_config.sin_port = htons(8888);// Chat-Port

    // Bind: attaches the socket to a specific IP and port
    if(bind(server_socket, (struct sockaddr *)&server_config, sizeof(server_config)) == SOCKET_ERROR) {
        printf("Bind failed! Error: %d\n", WSAGetLastError());
        return 1;
    }
    listen(server_socket, 3);
    printf("Server started on Port 8888...\n");

    // the connection loop
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_config, &client_config_size);

        if (client_socket != INVALID_SOCKET) {
            WaitForSingleObject(mutex, INFINITE);
            if(client_count < MAX_CLIENTS) {
                clients[client_count++] = client_socket;

                // Starting a thread to listen to this specific client
                CreateThread(NULL, 0, handle_client, (LPVOID)client_socket, 0, NULL);
            }
            ReleaseMutex(mutex);
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

DWORD WINAPI handle_client(LPVOID lpParam) {
    SOCKET s = (SOCKET)lpParam;
    char buffer[1024];
    char name[32];
    int bytes_recv;

    // Receive name of the client first
    bytes_recv = recv(s, name, sizeof(name) - 2, 0);
    if(bytes_recv > 0) {
        name[bytes_recv] = '\0';
        printf("[SERVER] %s has arrived!\n", name);

        char arrival_msg[100];
        sprintf(arrival_msg, "--- %s joined the chat ---\n", name);

        WaitForSingleObject(mutex, INFINITE);
        for(int i = 0; i < client_count; i++) {
            if(clients[i] != s) send(clients[i], arrival_msg, strlen(arrival_msg), 0);
        }
        ReleaseMutex(mutex);
    }

    while(1) {
        bytes_recv = recv(s, buffer, sizeof(buffer), 0);

        if(bytes_recv <= 0) {
            printf("A client disconnected.\n");
            break;
        }

        buffer[bytes_recv] = '\0';
        printf("%s", buffer);

        WaitForSingleObject(mutex, INFINITE);
        for(int i = 0; i < client_count; i++) {
            if(clients[i] != s) {
                send(clients[i], buffer, bytes_recv, 0);
            }
        }
        ReleaseMutex(mutex);
    }
    closesocket(s);
    return 0;
}