# Activity 3 : Process concepts
## Group: wget
1. กณวรรธน์ วิลาศรี
2. จ
3. ก

### Q1
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int compute_period = 5;
int sleep_period = 5;
int i;

/* what to do when alarm is on */
void on_alarm (int signal) {
    printf("Sleep\n");
    sleep (sleep_period);
    printf ("Wake up\n");
    /* activate alarm again */
    alarm (compute_period);
}

main (int argc, char* argv[])
{
    int i;
    if (argc != 3) {
        printf("Enter compute period : \n");
        scanf("%d", &compute_period);
        printf("Enter sleep period : \n");
        scanf("%d", &sleep_period);
    }
    else {
        compute_period = atoi (argv[1]);
        sleep_period = atoi (argv[2]);
    }
    /* on_alarm() is signal handler for SIGALARM */
    signal(SIGALRM, on_alarm);
    /* activate alarm */
    alarm (compute_period);
    /* compute infinitely but can be interrupted by alarm */
    for (i=0; ;i++) {
        if (i==0) printf("computing\n");
    }
}
```
ให้ผู้ใช้กำหนดบอกจำนวน และเก็บตัวแปร
### Q2
```c
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
        if (childpid > 0) {
            wait(NULL);
            break;
        }
    }
    printf("This is procepss %ld with parent %ld\n", (long)getpid(), (long)getppid());
    wait(0);
}
```
เพิ่ม `wait(NULL)` เพื่อให้ parent รอ child process จบก่อน
### Q3
```c
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
        childpid = fork() ; // parallel universe, childpid == 0; child, childpid > 0; parent
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
```
ทำให้ child จบ process ก่อน parent