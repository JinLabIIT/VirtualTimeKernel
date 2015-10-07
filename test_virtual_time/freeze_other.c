#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

#include "util.h"

void issue_freeze(pid_t *pid_list, size_t size, int wait, int pause)
{
        long int i, usec;
        struct timeval prev, next, diff;
        if ( pid_list && size ) {
                for ( i = 0; i < wait * CNT_SLEEP; ++i) {
                        // do nothing
                }
                gettimeofday(&prev, NULL);
                freeze_all_procs(pid_list, size);
                for ( i = 0; i < pause * CNT_SLEEP; ++i ) {
                        // pause for a while
                }
                unfreeze_all_procs(pid_list, size);
                gettimeofday(&next, NULL);
                timeval_substract(&diff, &next, &prev);
                usec = timeval_to_usec(diff);
                printf("I make all %lu processes freeze %ld micro_sec\n", size, usec);
        }
}

void issue_unfreeze(pid_t *pid_list, size_t size)
{

}

#define MAX_NUM_PIDS 256

int main(int argc, char** argv)
{
        int pid_found, opt, index, i, wait, freeze;
        int pid_list[MAX_NUM_PIDS];
        size_t pid_size;
        char *next;
        const char* const short_options = "np:w:f:";
        const struct option long_options[] = {
                {"nop", 0, 0, 0},
                {"pid", 1, 0, 0}
        };

        pid_found = 0;
        pid_size = 0;
        wait = 5;
        freeze = 15;
        do {
                opt = getopt_long(argc, argv, short_options, long_options, NULL);
                switch (opt) {
                        case 'n': 
                                break;
                        case 'p':
                                pid_found = 1;
                                index = optind - 1;
                                while (index < argc) {
                                        next = strdup(argv[index++]);
                                        if (next[0] != '-') {
                                                pid_list[pid_size++] = atoi(next);
                                        } else break;
                                }
                                optind = index - 1;
                                break;
                        case 'w':
                                wait = atoi(optarg);
                                /*printf("got wait = %d\n", wait);*/
                                break;
                        case 'f':
                                freeze = atoi(optarg);
                                /*printf("got freeze = %d\n", freeze);*/
                                break;
                        case -1:
                                break;
                        default:
                                printf("Usage: %s -w wait_factor -f freeze_factor -p pid1 pid2 pid3 ...\n", argv[0]);
                                exit(EXIT_FAILURE);
                }
        } while (opt != -1);
        if ( pid_found == 1 ) {
                /*for (i = 0; i < pid_size; ++i) {*/
                     /*printf("pid_list[%d] = %d\n", i, pid_list[i]);*/
                /*}*/
                issue_freeze(pid_list, pid_size, wait, freeze);
        }
        return 0;
}

