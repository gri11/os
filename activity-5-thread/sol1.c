#include <stdio.h>
#include <pthread.h>
void *say_hello(void *data)
{
    char *str;
    str = (char *)data;
    while (1)
    {
        printf("%s\n", str);
        sleep(1);
    }
}

void main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("usage : sol1 arg1 arg2\n");
        return;
    }
    pthread_t t1, t2;
    pthread_create(&t1, NULL, say_hello, *(argv + 1));
    pthread_create(&t2, NULL, say_hello, *(argv + 2));
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}