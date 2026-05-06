#include <winsock2.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char theTarget[32];
    int startPort;
    int endPort;
} ScanData;

HANDLE printMutex;

int userInputHandle();
int threadsHandle(ScanData *mainData);
void *socketScan(void *value);

int main() {
    printMutex = CreateMutex(NULL, FALSE, NULL);
    userInputHandle();
    CloseHandle(printMutex);
    return 0;
}

int userInputHandle() {
    WSADATA wsa;
    char input[50];
    ScanData *data = malloc(sizeof(ScanData));

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d", WSAGetLastError());
        return 1;
    }

    system("cls");
    printf("Targets IP: ");
    if (fgets(data->theTarget, sizeof(data->theTarget), stdin) == NULL) {
        perror("fgets failed");
        return 1;
    }
    data->theTarget[strcspn(data->theTarget, "\n")] = '\0';

    system("cls");
    printf("Start Port: ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *endPtr;
        long val = strtol(input, &endPtr, 10);
        if (input == endPtr) { printf("Invalid input!\n"); return 1; }
        if (val < 1 || val > 65535) { printf("Port out of range\n"); return 1; }
        data->startPort = (int)val;
    } else { perror("fgets failed"); return 1; }

    system("cls");
    printf("End Port: ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *endPtr;
        long val = strtol(input, &endPtr, 10);
        if (input == endPtr) { printf("Invalid input!\n"); return 1; }
        if (val < 1 || val > 65535) { printf("Port out of range\n"); return 1; }
        data->endPort = (int)val;

        if (data->startPort > data->endPort) {
            printf("Start port must be <= end port\n");
            free(data);
            return 1;
        }

        threadsHandle(data);
        free(data);
        return 0;
    } else { perror("fgets failed"); return 1; }
}

int threadsHandle(ScanData *mainData) {
    int totalPorts = mainData->endPort - mainData->startPort + 1;
    int numThreads = (totalPorts < 100) ? totalPorts : 100;
    pthread_t threads[100];

    int portsPerThread = totalPorts / numThreads;
    int remainder = totalPorts % numThreads;
    int currentStart = mainData->startPort;

    system("cls");
    printf("Scanning %s (%d - %d)...\n", mainData->theTarget, mainData->startPort, mainData->endPort);
    printf("\n-------------------------------\n");

    for (int i = 0; i < numThreads; i++) {
        ScanData *threadData = malloc(sizeof(ScanData));
        strcpy(threadData->theTarget, mainData->theTarget);
        threadData->startPort = currentStart;

        int extra = (i < remainder) ? 1 : 0;
        threadData->endPort = currentStart + portsPerThread + extra - 1;
        if (threadData->endPort > mainData->endPort)
            threadData->endPort = mainData->endPort;

        pthread_create(&threads[i], NULL, socketScan, (void *)threadData);

        currentStart = threadData->endPort + 1;
    }

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);

    return 0;
}

void *socketScan(void *value) {
    SOCKET s;
    struct sockaddr_in target;
    ScanData *data = (ScanData *)value;

    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(data->theTarget);

    for (int i = data->startPort; i <= data->endPort; i++) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            printf("Socket error: %d", WSAGetLastError());
            break;
        }

        target.sin_port = htons(i);

        u_long mode = 1;
        ioctlsocket(s, FIONBIO, &mode);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        fd_set writefds, exceptfds;
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        FD_SET(s, &writefds);
        FD_SET(s, &exceptfds);

        int res = connect(s, (struct sockaddr *)&target, sizeof(target));
        if (res == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                closesocket(s);
                continue;
            }
        }

        if (select(0, NULL, &writefds, &exceptfds, &timeout) > 0) {
            int err, len = sizeof(err);
            getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
            if (err == 0) {
                WaitForSingleObject(printMutex, INFINITE);
                printf("[+] Port %d is open\n", i);
                ReleaseMutex(printMutex);
            }
        }

        closesocket(s);
    }

    free(data);
    return 0;
}