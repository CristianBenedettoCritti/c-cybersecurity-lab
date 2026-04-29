#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")


typedef struct {
    char theTarget[32];
    int startPort;
    int endPort;
} ScanData;

// IMPORTANT
// "." => struct variable
// "->" => pointer to struct

// Global mutex to prevent threads from printing at the same time (race condition fix)
HANDLE printMutex;

int userInputHandle();
int threadsHandle(ScanData *mainData);
DWORD WINAPI socketScan(LPVOID lpParam);

int main() {
    // Create the mutex before anything runs
    printMutex = CreateMutex(NULL, FALSE, NULL);
    userInputHandle();
    // Clean up the mutex when done
    CloseHandle(printMutex);
    return 0;
}

int userInputHandle() {
    WSADATA wsa;
    char input[50];
    ScanData *data = malloc(sizeof(ScanData));

    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d", WSAGetLastError());
        return 1;
    }
    
    system("cls");
    printf("Targets IP: ");
    if(fgets(data->theTarget, sizeof(data->theTarget), stdin) == NULL) {
        perror("fgets failed");
        return 1;
    }

    data->theTarget[strcspn(data->theTarget, "\n")] = '\0';

    system("cls");
    printf("Start Port: ");
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

        data->startPort = (int)val;
    } else {
        perror("fgets failed");
        return 1;
    }

    system("cls");
    printf("End Port: ");
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

        data->endPort = (int)val;
        threadsHandle(data);
        free(data);
        return 0;
    } else {
        perror("fgets failed");
        return 1;
    }

}

int threadsHandle(ScanData *mainData) {
    int totalPorts = mainData->endPort - mainData->startPort + 1;
    int numThreads = (totalPorts < 100) ? totalPorts : 100;
    HANDLE threads[100];

    int portsPerThread = totalPorts / numThreads;
    int remainder = totalPorts % numThreads;
    int currentStart = mainData->startPort;

    system("cls");
    printf("Scanning %s (%d - %d)...\n", mainData->theTarget, mainData->startPort, mainData->endPort);
    printf("\n-------------------------------\n");

    // Gives each thread his own ScanData space of memory
    for(int i = 0; i < numThreads; i++) {
        ScanData *threadData = malloc(sizeof(ScanData));
        strcpy(threadData->theTarget, mainData->theTarget);

        // Give it a specific start and end port
        threadData->startPort = currentStart;

        // Spread the remainder evenly
        int extra = (i < remainder) ? 1 : 0;
        threadData->endPort = currentStart + portsPerThread + extra - 1;

        if(threadData->endPort > mainData->endPort) {
            threadData->endPort = mainData->endPort;
        }

        threads[i] = CreateThread(NULL, 0, socketScan, (LPVOID)threadData, 0, NULL);

        currentStart = threadData->endPort + 1;
    }

    // WaitForMultipleObjects max limit is 64 on Windows (MAXIMUM_WAIT_OBJECTS)
    // Fix: wait in batches of 64
    int remaining = numThreads;
    int offset = 0;
    while(remaining > 0) {
    //  int batchSize = (is remaining bigger than MAXIMUM_WAIT_OBJECTS(64)) ?(if yes) :(else);
        int batchSize = (remaining > MAXIMUM_WAIT_OBJECTS) ? MAXIMUM_WAIT_OBJECTS : remaining;
    //  WaitForMultipleObjects(how many threads, where to begin(Pointer-Arithmetic), TRUE(WAIT ALL), Time limit);
        WaitForMultipleObjects(batchSize, threads + offset, TRUE, INFINITE);
    //  Marking that we have done 64 threads already
        offset += batchSize;
    //  We take away the threads we have already done from the remaining
        remaining -= batchSize;
    }

    // Wait for all threads to finish
    for(int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }
    return 0;
}

DWORD WINAPI socketScan(LPVOID lpParam) {
    SOCKET s;
    struct sockaddr_in target;
    // data (ScanData*) = pointer to the struct passed in lpParam
    ScanData *data = (ScanData *)lpParam;

    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(data->theTarget);

    for(int i = data->startPort; i <= data->endPort; i++) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if(s == INVALID_SOCKET) {
            printf("Socket error: %d", WSAGetLastError());
            break;
        }

        target.sin_port = htons(i);

        u_long mode = 1;
        ioctlsocket(s, FIONBIO, &mode);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        fd_set writefds;
        // Added exceptfds to catch failed connections on Windows
        // On Windows, failed non-blocking connects appear in exceptfds, not writefds
        fd_set exceptfds;
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        FD_SET(s, &writefds);
        FD_SET(s, &exceptfds);

        int res = connect(s, (struct sockaddr *)&target, sizeof(target));

        if(res == SOCKET_ERROR) {
            int err = WSAGetLastError();

            if(err != WSAEWOULDBLOCK) {
                closesocket(s);
                continue;
            }
        }

        if(select(0, NULL, &writefds, &exceptfds, &timeout) > 0) {
            int err;
            int len = sizeof(err);

            getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &len);

            if(err == 0) {
            //  with mutex a thread can say that only he can print for now and so threads cant print at the same time.
                WaitForSingleObject(printMutex, INFINITE);
                printf("[+] Port %d is open\n", i);
            // Here the mutex gets released so the other threads can print too again
                ReleaseMutex(printMutex);
            }
        }

        closesocket(s);
    }
    free(data);
    return 0;
}