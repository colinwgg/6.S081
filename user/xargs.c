#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int readline(char *new_argv[MAXARG], int cur_argc) {
    char buf[1024];
    int n = 0;
    while (read(0, buf + n, 1)) {
        if (n == 1023) {
            fprintf(2, "argument too long\n");
            exit(1);
        }
        if (buf[n] == '\n') {
            break;
        }
        n++;
    }
    if (n == 0) {
        return 0;
    }
    buf[n] = 0;
    int offset = 0;
    while (offset < n) {
        new_argv[cur_argc++] = buf + offset;
        while (buf[offset] != ' ' && offset < n) {
            offset++;
        }
        while (buf[offset] == ' ' && offset < n) {
            offset++;
        }
    }
    return cur_argc;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Usage: xargc <command> <argv[]>\n");
        exit(1);
    }
    char *command = argv[1];
    char *new_argv[MAXARG];
    for (int i = 1; i < argc; i++) {
        new_argv[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(new_argv[i - 1], argv[i]);
    }
    int cur_argc;
    while ((cur_argc = readline(new_argv, argc - 1)) != 0) {
        new_argv[cur_argc] = 0;
        if (fork() == 0) {
            exec(command, new_argv);
            fprintf(2, "exec failed\n");
            exit(1);
        } else {
            wait(0);
        }
    }
    exit(0);
}