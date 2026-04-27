#include <winsock2.h>   // The Windows Networking "Dictionary"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This tells the compiler to link the actual Windows Socket binary (DLL)
// Without this, it will get "Undefined Reference" errors
#pragma comment(lib, "ws2_32.lib")

int socketScan(char *theTarget, int startPort, int endPort);

int main() {
    char *theTarget = malloc(32);
    int startPort;
    int endPort;
    char input[50];
    
    printf("Targets IP: ");
    if(fgets(theTarget, 32, stdin) == NULL) {
        perror("fgets failed");
        free(theTarget);
        return 1;
    }

    system("cls");
    printf("Start port: ");
    if(fgets(input, sizeof(input), stdin) != NULL) {
        char *endPtr;
        long val = strtol(input, &endPtr, 10);

        if(input == endPtr) {
            printf("Invalid input, enter a number!");
            return 1;
        }

        if(val < 1 || val > 65535) {
            printf("Port out of range (1 - 65535)\n");
            return 1;
        }

        startPort = (int)val;
    }

    system("cls");
    printf("\nEnd port: ");
    if(fgets(input, sizeof(input), stdin) != NULL) {
        char *endPtr;
        long val = strtol(input, &endPtr, 10);

        if(input == endPtr) {
            printf("Invalid input, enter a number!");
            return 1;
        }

        if(val < 1 || val > 65535) {
            printf("Port out of range (1 - 65535)\n");
            return 1;
        }

        endPort = (int)val;
    }

    theTarget[strcspn(theTarget, "\n")] = '\0';

    socketScan(theTarget, startPort, endPort);
    free(theTarget);
    return 0;
}

int socketScan(char *theTarget, int startPort, int endPort) {
    WSADATA wsa;    // WSADATA = Windows fills it with details about your network setup
    SOCKET s;       // The Digital Telephone handle
    struct sockaddr_in target;  // The "GPS coordinates" for our scan

    // WSAStartup powers on the networking system
    // MAKEWORD(2, 2) = I want to use version 2.2 of WinSock
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d", WSAGetLastError());
        return 1;
    }

    // Defines the adress family (IPv4 or IPv6) in this case IPv4
    target.sin_family = AF_INET;

    // Converting the string into a format the network card understands (32-bit integer (in network byte order))
    target.sin_addr.s_addr = inet_addr(theTarget);
    system("cls");
    printf("IP: %s\nPort: %d - %d", theTarget, startPort, endPort);
    printf("\nOpen Ports:");

    for (int i = startPort; i <= endPort; i++) {
        // For every port, we create a new socket
        s = socket(AF_INET, SOCK_STREAM, 0);

        if(s == INVALID_SOCKET) {
            printf("Socket error: %d", WSAGetLastError());
            break;
        }

        // htons = Host to Network Short, networks read number to the opposit and switches the numbers so the computer gets the right port
        // We tell the target which port we are trying to connect in the moment
        target.sin_port = htons(i);

        u_long mode = 1; // 1 = on, 0 = off
        // ioctlsocket: Is the controller for the socket settings
        // FIONBIO: Stands for "File Input Output Non-Blocking I/O"
        ioctlsocket(s, FIONBIO, &mode); // turns on Non-Blocking

        struct timeval timeout;
        timeout.tv_sec = 0;       // tv_sec: The seconds part of our timer (we set it to 0)
        timeout.tv_usec = 100000; // tv_usec: The microseconds part (100,000 = 0.1 seconds)

        fd_set writefds;      // fd_set: A "bucket" or "checklist" that holds our sockets to watch
        FD_ZERO(&writefds);   // FD_ZERO: Empties the bucket so it is clean
        FD_SET(s, &writefds); // FD_SET: Puts our socket 's' into the bucket to be monitored

        // connect = simply does a handshake
        // connect = Client sends a SYN-Package an if the Port is open it answers with a SYN-ACK-Package and the client sends a ACK-Package
        // SYN (Synchronize) = Sends a request to connect to the server
        // SYN-ACK (Synchronize + Acknowledge) = (ACK) The server tells the client he got his request (SYN) The server tells the client he is ready for a connection
        // ACK (Acknowledge) = The client tells the server, the connection is up | IMPORTANT: We skip this step so we dont do the full handshake
        // We try to connect, but because of Non-Blocking mode, it returns immediately
        connect(s, (struct sockaddr *)&target, sizeof(target));

        // select: The "Timer" function. It waits for the socket in the bucket to become "writable" (connected)
        // It stops waiting if the 'timeout' time be reached
        // If it returns > 0, it means the connection succeeded within the time limit
        if(select(0, NULL, &writefds, NULL, &timeout) > 0) {
            printf("\n%d", i);
        }

        // IMPORTANT: After each try wie have to close the socket
        closesocket(s);
    }

    WSACleanup();
    return 0;
}