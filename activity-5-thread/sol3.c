#include <stdio.h>
#include <pthread.h>
void *say_hello(void *data)
{
    char *str;
    str = (char *)data;
    printf("%s\n", str);
    sleep(1);
}

void main()
{
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        pthread_t t1, t2;
        pthread_create(&t1, NULL, say_hello, "First thread from child process");
        pthread_create(&t2, NULL, say_hello, "Second thread from child process");

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
    }
    else
    {
        pthread_t t1, t2;
        pthread_create(&t1, NULL, say_hello, "First thread from parent process");
        pthread_create(&t2, NULL, say_hello, "Second thread from parent process");

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
    }
}