#include <stdio.h>

void *computation(void *value);

int main() {
    long value1 = 1;
    long value2 = 2;

    computation( (void*) &value1);
    computation( (void*) &value2);

    return 0;
}

void *computation(void *value) {
    long sum = 0;
    // You convert the generic pointer back to a long *
    // Now value_num points to value1
    long *value_num = (long *) (value);

    for(long i = 0; i < 1000000000; i++)
        sum += *value_num;
    printf("Add: %d\n", *value_num);
    return NULL;
}