#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>		// for getopt()
#include <time.h>		// for timeval
#include <unistd.h>		// for getpid()
#include <sys/types.h>

#include "vtutil.h"

unsigned long elapsed[NR_ACCU_ROUND];
unsigned long dilated_elapsed[NR_ACCU_ROUND];

void fill_elapsed(int duration)
{
        struct timeval prev, next, diff, temp;
        int ret;
        long int i, j, usec;

        for (i = 0; i < NR_ACCU_ROUND; ++i) {
                gettimeofday(&prev, NULL);
                usleep(duration);
                /*for (j = 0; j < CNT_SLEEP * factor; ++j) {*/
                        /*[>gettimeofday(&temp, NULL);<]*/
                /*}*/
                gettimeofday(&next, NULL);
                timeval_substract(&diff, &next, &prev);
                usec = timeval_to_usec(diff);
                elapsed[i] = usec;
        }
}

void fill_dilated_elapsed(int dil, int duration)
{
        struct timeval prev, next, diff, temp;
        int ret;
        long int usec, i, j;
        pid_t pid = getpid();

        ret = virtual_time_unshare(CLONE_NEWNET | CLONE_NEWNS);
        set_new_dilation(pid, dil);
        /*show_proc_dilation(pid);*/

        for (i = 0; i < NR_ACCU_ROUND; ++i) {
                gettimeofday(&prev, NULL);
                usleep(duration);
                /*for (j = 0; j < CNT_SLEEP * factor; ++j) {*/
                /*[>gettimeofday(&temp, NULL);<]*/
                /*}*/
                gettimeofday(&next, NULL);
                timeval_substract(&diff, &next, &prev);
                usec = timeval_to_usec(diff);
                dilated_elapsed[i] = usec;
        }
        ret = virtual_time_exit(pid);
}

void actual_dilation(FILE* output, int dil, float per_accu, int err_accu)
{
        long i;
        /*float q, percentage;*/
        /*long err;*/
        /*int per_count, err_count;*/
        /*per_count = err_count = 0;*/

        for (i = 0; i < NR_ACCU_ROUND; ++i) {
                /*q = (float)elapsed[i] / (float)dilated_elapsed[i];*/
                /*err = abs(elapsed[i] - dil * dilated_elapsed[i]); */
                /*if ((q - dil) * (q - dil) > per_accu * per_accu) ++per_count;*/
                /*if (err > err_accu) ++err_count;*/
                fprintf(output, "%lu\t%lu\n", elapsed[i], dilated_elapsed[i]);
        }
        /*percentage = (float)err_count / NR_ACCU_ROUND * 100;*/
        /*printf("[summary] %d (%.2f%%) bad dilations\n", err_count, percentage);*/
}

int main(int argc, char* const argv[])
{
        const char* const short_options = "t:edpu:b:o:";
        const struct option long_options[] = {
                {"tdf", 1, NULL, 't'},
                {"elapsed", 0, NULL, 'e'},
                {"dilated", 0, NULL, 'd'},
                {"print", 0, NULL, 'p'},
                {"duration", 1, NULL, 'u'},
                {"bound", 1, NULL, 'b'},
                {"output", 1, NULL, 'o'},
                {NULL, 0, NULL, 0},
        };
        int next_option;
        int run_elapsed = 0;
        int run_dilated = 0;
        int print_dil = 0;
        int dilation = 1;
        int duration = 1000;
        int err_usec = 900; // error bound in micro seconds
        char *output_filename;
        FILE *output_stream;

        while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
                switch(next_option) {
                        case 't':
                                dilation = atoi(optarg);
                                break;
                        case 'e':
                                run_elapsed = 1;
                                break;
                        case 'd':
                                run_dilated = 1;
                                break;
                        case 'p':
                                if (run_elapsed == 1 && run_dilated == 1)
                                        print_dil = 1;
                                break;
                        case 'u':
                                duration = atoi(optarg);
                                break;
                        case 'b':
                                err_usec = atoi(optarg);
                                break;
                        case 'o':
                                output_filename = malloc(sizeof(optarg) + 1);
                                strcpy(output_filename, optarg);
                                break;
                        default:
                                printf("Usage: accu_dilation -t tdf -u duration -o output_file [-e] [-d] [-p]\n");
                                exit(1);
                }
        }

        if (run_elapsed) fill_elapsed(duration);
        /*printf("Run fill_elapsed() parameters\n");  */
        if (run_dilated) fill_dilated_elapsed(dilation, duration);
        /*printf("Run fill_dilated_elapsed() parameters\n"); */
        if (print_dil) {
                output_stream = fopen(output_filename, "w");
                actual_dilation(output_stream, dilation, 0.1f, err_usec);
                fclose(output_stream);
        }
        return 0;
}
