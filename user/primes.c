#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const int INT_SIZE = sizeof(int);

int lpipe_read_first(int lpipe[2], int *dst) {
    if (read(lpipe[RD], dst, INT_SIZE) == INT_SIZE) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return 1;
}

void transmit_data(int lpipe[2], int rpipe[2], int first) {
    int data;
    while (read(lpipe[RD], &data, INT_SIZE) == INT_SIZE) {
        if (data % first != 0) {
            write(rpipe[WR], &data, INT_SIZE);
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;
    if (lpipe_read_first(lpipe, &first) == 0) {
        int rpipe[2];
        pipe(rpipe);
        transmit_data(lpipe, rpipe, first);

        int sonfd = fork();
        if (sonfd == 0) {
            primes(rpipe);
        } else {
            close(rpipe[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    int lpipe[2];
    pipe(lpipe);

    for (int i = 2; i <= 35; ++i) {
        write(lpipe[WR], &i, INT_SIZE);
    }

    int fd = fork();
    if (fd == 0) {
        primes(lpipe);
    } else {
        close(lpipe[WR]);
        close(lpipe[RD]);
        wait(0);
    }
    exit(0);
}