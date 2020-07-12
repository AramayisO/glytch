#ifndef CHILD_PROC_H
#define CHILD_PROC_H

/**
 * Initialize dist info file from command line arguments.
 * 
 * @param argc number of arguments in argv.
 * @param argv array of command line arguments.
 * @return On success, returns 0. On error, returns -1.
 */
int init_dist_info(int argc, char *argv[]);

/**
 * Function called by child process which simply allocates a random ammount of
 * memory chosen from a normal distribution.
 */
void child_proc(void);

#endif