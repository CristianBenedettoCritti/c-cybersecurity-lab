/*
=================================================
    DIFFERENCES BETWEEN pthread vs WinAPI VERSION
=================================================

-----------------------------------------------
1. INCLUDES
-----------------------------------------------

pthread version:
    #include <pthread.h>

WinAPI version:
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")

-----------------------------------------------
2. THREAD ARRAY
-----------------------------------------------

pthread version:
    pthread_t threads[100];

WinAPI version:
    HANDLE threads[100];

-----------------------------------------------
3. FUNCTION DECLARATION
-----------------------------------------------

pthread version:
    void *socketScan(void *value);

WinAPI version:
    DWORD WINAPI socketScan(LPVOID lpParam);

-----------------------------------------------
4. FUNCTION DEFINITION
-----------------------------------------------

pthread version:
    void *socketScan(void *value) {

WinAPI version:
    DWORD WINAPI socketScan(LPVOID lpParam) {

-----------------------------------------------
5. PARAMETER CAST INSIDE socketScan
-----------------------------------------------

pthread version:
    ScanData *data = (ScanData *)value;

WinAPI version:
    ScanData *data = (ScanData *)lpParam;

-----------------------------------------------
6. THREAD CREATION
-----------------------------------------------

pthread version:
    pthread_create(&threads[i], NULL, socketScan, (void *)threadData);

WinAPI version:
    threads[i] = CreateThread(NULL, 0, socketScan, (LPVOID)threadData, 0, NULL);

-----------------------------------------------
7. THREAD JOINING
-----------------------------------------------

pthread version:
    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);

WinAPI version:
    int remaining = numThreads;
    int offset = 0;
    while(remaining > 0) {
        int batchSize = (remaining > MAXIMUM_WAIT_OBJECTS) ? MAXIMUM_WAIT_OBJECTS : remaining;
        WaitForMultipleObjects(batchSize, threads + offset, TRUE, INFINITE);
        offset += batchSize;
        remaining -= batchSize;
    }
    for(int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }

-----------------------------------------------
8. startPort > endPort VALIDATION
-----------------------------------------------

pthread version:
    if (data->startPort > data->endPort) {
        printf("Start port must be <= end port\n");
        free(data);
        return 1;
    }

WinAPI version:
    (not present)
*/
