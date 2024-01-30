#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
main()
{
    int i;
    int n;
    pid_t childpid; 
    n = 4;
    for (i = 0; i < n; ++i) {
        childpid = fork() ; // parallel universe, childpid == 0 // child, childpid > 0 // parent
        if (childpid == 0) {
            exit(0);
        }
        else {
            sleep(50);
        }
        wait(0);
    }
    printf("This is process %ld with parent %ld\n",(long) getpid (), (long)
    getppid());
}