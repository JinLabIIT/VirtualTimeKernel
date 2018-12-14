#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vtutil.h"

#define MAX_NUM_PIDS 512

#define SHOW_OVHD 1

/**
 * Co-Author channon@iit.edu
 * Listens on /tmp/fifo.tmp for a signal
 * CLA: pids space delimited
 */
int main (int argc, char **argv){
        char buf[1];
        pid_t pid;
        pid_t pid_list[MAX_NUM_PIDS];
        size_t pid_size = argc - 1; // -1 because program name is argv[0]
        int fd;
        /* read length */
        int l, i;
        /* create FIFO */
        char *filename = "/tmp/fifo.tmp";

        for (i = 1; i < argc; i++) {
                pid_list[i-1] = atoi(argv[i]);
        }
        fd = open(filename, O_RDONLY);
        while (1){
                l = read(fd, buf, 1);
                if (l) {
                        if (buf[0] == 'p') {
                                kickoff_pthreads_freeze(pid_list, pid_size, freeze_work, NULL);
                        } else {
                                kickoff_pthreads_freeze(pid_list, pid_size, unfreeze_work, NULL);
                        }
                }

        }

        /* close and delete FIFO */
        close(fd);
}
