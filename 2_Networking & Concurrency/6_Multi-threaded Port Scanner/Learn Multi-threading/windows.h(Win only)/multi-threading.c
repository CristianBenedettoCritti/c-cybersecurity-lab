#include <stdio.h>
// windows.h gives us CreateThread, HANDLE, WaitForSingleObject
#include <windows.h>

// a windows api thread must:
//      1. Accept one pointer argument (LPVOID)
//      2. Return a 32-bit integer result (DWORD)
//      3. Follow a specific calling convention (WINAPI)

// DWORD = 32-bit unsigned integer
// WINAPI = calling convention (how arguments are passed in memory)
/*
    ADVANCED EXPLANATION FOR WINAPI:
    WINAPI = __stdcall | __stdcall = convention
    This convetion means:
    Rule 1: callee cleans the stack
        The function itself removes arguments from the stack when it returns.
    Rule 2: fixed calling format
        Arguments are pushed in a strict order.
    Rule 3: consistent binary layout
        So Windows DLL's can call your funciton safely.
*/
// LPVOID = generic pointer (void*)
DWORD WINAPI computation(LPVOID value);

int main() {
    // HANDLE = type (like int, char) that represents a thread
    HANDLE thread_1;
    HANDLE thread_2;

    long value1 = 1;
    long value2 = 2;

/* 
CreateThread(
  LPSECURITY_ATTRIBUTES   lpThreadAttributes = security / inheritance settings | NULL → default settings
  SIZE_T                  dwStackSize = sets the stack size (memory) for the thread | 0 → use default stack size (usually 1 MB),
  LPTHREAD_START_ROUTINE  lpStartAddress = function the thread will run,
  LPVOID                  lpParameter = argument passed into your thread function,
  DWORD                   dwCreationFlags = controls how the thread starts | 0 → run immediately,
  LPDWORD                 lpThreadId = receives the thread ID output | NULL → don’t care about the ID
);
*/
    thread_1 = CreateThread(NULL, 0, computation, &value1, 0, NULL);
    thread_2 = CreateThread(NULL, 0, computation, &value2, 0, NULL);

    // WaitForSingleObject(The thread to wait for, how long to wait);
    // INFINITE = Until the thread is finished
    WaitForSingleObject(thread_1, INFINITE);
    WaitForSingleObject(thread_2, INFINITE);

//  IMPORTANT!!!
//   Windows keeps kernel object alive until they get closed,
//   so you have to close them!!!
    CloseHandle(thread_1);
    CloseHandle(thread_2);

    return 0;
}

DWORD WINAPI computation(LPVOID value) {
    // You convert the generic pointer back to a long *
    // Now value_num points to value1
    long *value_num = (long*)value;
    printf("Add: %d\n", *value_num);
    return (DWORD)(*value_num);
}