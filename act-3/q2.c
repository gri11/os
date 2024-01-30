#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
main()
{
    int i;
    int n;
    pid_t childpid;
    n = 4;
    for (i = 0; i < n; ++i) {
        childpid = fork() ;
        // printf("%d %d ", i, getpid());
        // printf("\n");
        if (childpid > 0) {
            wait(NULL);
            // printf("%d\n", i);
            break;
        }
    }
    // printf("%d ", getpid());
    printf("This is process %ld with parent %ld\n", (long)getpid(), (long)getppid());
    wait(0);
}