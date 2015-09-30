#include <errno.h>	// for strerror
#include <string.h>
#include <sched.h>	// for unshare
#include <stdio.h> 	// for file operations
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>	// for PATH_MAX

#include "util.h"

/**
 * General helpers for error checking and time calculation
 **/
long check_syscall_status(long ret, char* syscall_name)
{
    if(ret) {
        printf("\n[error] %s fails with error: %s\n", 
			syscall_name, strerror(errno));
        exit(errno);
    }
    return ret;
}

int timeval_substract(struct timeval* result, 
		struct timeval* x, struct timeval* y)
{
    if (x->tv_usec < y->tv_usec )
    {
        int nsec = (y->tv_usec - x->tv_usec) / USEC_PER_SEC + 1;
        y->tv_usec -= USEC_PER_SEC * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > USEC_PER_SEC)
    {
        int nsec = (x->tv_usec - y->tv_usec) / USEC_PER_SEC + 1;
        y->tv_usec += USEC_PER_SEC * nsec;
        y->tv_sec -= nsec;
    }
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    return x->tv_sec > y->tv_sec;
}

long int timeval_to_usec(struct timeval tv)
{
    return tv.tv_sec * USEC_PER_SEC + tv.tv_usec;
}

#define TDF_MAX 4 			// 3 digit number
#define CLONE_NEWTIME 0x40000000
#define _GNU_SOURCE 			// for unshare system call

int virtual_time_unshare(int flags)
{
	int ret;
	
	ret = unshare(flags | CLONE_NEWTIME);
	return check_syscall_status(ret, "unshare");
}

int set_new_dilation(pid_t pid, int tdf)
{
	int proc_file;
	char path[PATH_MAX];
	char* tdf_str;
	size_t count;
	ssize_t written_count = 0;

	if ( tdf < TDF_MAX ){
		sprintf(path, "/proc/%d/dilation", pid);
		proc_file = open(path, O_WRONLY);
		if ( proc_file == -1 ) {
			printf("cannot open %s with error %s\n", 
					path, strerror(errno));
			return -1;
		}
		tdf_str = malloc(sizeof(char) * TDF_MAX);
		if ( tdf_str == NULL ) {
			return -1;
		}
		count = sprintf(tdf_str, "%d", tdf);
		// count should equal strlen(tdf_str)
		written_count = write(proc_file, tdf_str, count);
		close(proc_file);
		free(tdf_str);
	}
	return written_count;
}

int virtual_time_exit(pid_t pid)
{
	return set_new_dilation(pid, 0);
}

static int write_proc_freeze(pid_t pid, char* val)
{
	int proc_file;
	char path[PATH_MAX];
	int written_count = 0;

	sprintf(path, "/proc/%d/freeze", pid);
	proc_file = open(path, O_WRONLY);
	if ( proc_file == -1 ) {
		printf("cannot open %s with error %s\n", 
				path, strerror(errno));
		return -1;
	}
	written_count = write(proc_file, val, 1);
	close(proc_file);
	return written_count;
}

int freeze_proc(pid_t pid)
{
	char *val = "1";
	return write_proc_freeze(pid, val);
}

int unfreeze_proc(pid_t pid)
{
	char *val = "0";
	return write_proc_freeze(pid, val);
}





