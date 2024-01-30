#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Error : cannot fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // printf("%s\n", argv[0]);
        if (argc == 1)
        {
            printf("Please enter UNIX command\n");
        }
        execvp(argv[1], argv + 1);
    }
    else
    {
        wait(NULL);
        return (0);
    }
}