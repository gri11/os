#include <stdio.h>
#include <pthread.h>
void *say_hello(void *data)
{
    int *str;
    str = (int *)data;
    printf("This is thread %d\n", *str + 1);
    sleep(1);
}

void main()
{
    pthread_t ts[10];
    for (int i = 0; i < 10; i++)
    {
        pthread_create(ts + i, NULL, say_hello, &i);
        pthread_join(*(ts + i), NULL);
    }
}