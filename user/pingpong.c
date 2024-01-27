#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int childToParent[2];
    int parentToChild[2];
    char buf = 'P';
    int exit_status = 0;

    pipe(childToParent);
    pipe(parentToChild);
    int pid = fork();
    
    if (pid < 0) {
        fprintf(2, "fork() error\n");
        close(parentToChild[1]);
        close(childToParent[0]);
        close(parentToChild[0]);
        close(childToParent[1]);
        exit(1);
    } else if (pid == 0) {
        close(parentToChild[1]); // write closed
        close(childToParent[0]); // read closed

        if (read(parentToChild[0], &buf, 1) == -1) {
            fprintf(2, "child read() error!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        if (write(childToParent[1], &buf, 1) == -1) {
            fprintf(2, "child write() error!\n");
            exit_status = 1;
        }

        close(parentToChild[0]);
        close(childToParent[1]);
        exit(exit_status);
    } else {
        close(parentToChild[0]);
        close(childToParent[1]);

        if (write(parentToChild[1], &buf, 1) == -1) {
            fprintf(2, "parent write error\n");
            exit_status = 1;
        }
        if (read(childToParent[0], &buf, 1) == -1) {
            fprintf(2, "parent read error\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(parentToChild[1]);
        close(childToParent[0]);
        exit(exit_status);
    }
}