#include <stdio.h>
#include <pthread.h>

// void function with a generic pointer (void *)
// a generic pointer can point to any type of data
void *computation(void *value);

int main() {
    // pthread_t = type (like int, char) that represents a thread
    pthread_t thread1;
    pthread_t thread2;

    long value1 = 1;
    long value2 = 2;

    // pthread_create(pointer to pthread_t variable, set attributes, the function the thread will run, argument passed to the thread (must be a generic pointer))
    pthread_create(&thread1, NULL, computation, (void*) &value1);
    pthread_create(&thread2, NULL, computation, (void*) &value2);

    // pthread_join(The thread to join, work with the return value of the function)
    // This makes main wait until thread1 finishes
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

void *computation(void *value) {
    // You convert the generic pointer back to a long *
    // Now value_num points to value1
    long *value_num = (long *) (value);
    printf("Add: %d\n", *value_num);
    return NULL;
}