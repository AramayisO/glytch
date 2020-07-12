#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/child_proc.h"
#include "../include/mem_util.h"
#include "../include/rand_util.h"

//-----------------------------------------------------------------------------
// File describing the distributions to use. The columns of the file are:
// (1) current iteration (used to determine when to switch distributions)
// (2) threshold iteration to switch to using second distribution
// (3) mean of first distribution 
// (4) standard deviation of first distribution
// (5) mean of second distribution
// (6) standard deviation of second distribution
//-----------------------------------------------------------------------------
const char *DIST_INFO_FILEPATH = "data/dist.info";

//-----------------------------------------------------------------------------
// Structure representing the data stored in the dist info file.
// See DIST_INFO_FILEPATH for description of the fields.
//-----------------------------------------------------------------------------
struct dist_info_t
{
    long iter;
    long thresh;
    long mu_1;
    long sigma_1;
    long mu_2;
    long sigma_2;
};

//-----------------------------------------------------------------------------
// Parses the data/dist.info file in order to determine the mean and standard
// deviation of the first and second distributions, and the number of times 
// the function has been called. Based on this information, the function
// returns the number of bytes the child_proc function should allocate.
//-----------------------------------------------------------------------------
static size_t num_bytes_to_alloc(void)
{
    struct dist_info_t dist_info;
    size_t num_pages;
    FILE *fd_dist_info;
    
    if ((fd_dist_info = fopen(DIST_INFO_FILEPATH, "r+")) == NULL)
    {
        printf("Error: [num_bytes_to_alloc] failed to open %s\n", DIST_INFO_FILEPATH);
        exit(EXIT_FAILURE);
    }
    else
    {
        fscanf(fd_dist_info, "%ld %ld %ld %ld %ld %ld",
            &dist_info.iter, 
            &dist_info.thresh,
            &dist_info.mu_1,
            &dist_info.sigma_1,
            &dist_info.mu_2, 
            &dist_info.sigma_2);
        
        rewind(fd_dist_info);

        fprintf(fd_dist_info, "%ld %ld %ld %ld %ld %ld",
            dist_info.iter + 1, 
            dist_info.thresh,
            dist_info.mu_1,
            dist_info.sigma_1,
            dist_info.mu_2,
            dist_info.sigma_2);

        fclose(fd_dist_info);
    }
    
    num_pages = (size_t) (dist_info.iter < dist_info.thresh
        ? norm_rand(dist_info.mu_1, dist_info.sigma_1) 
        : norm_rand(dist_info.mu_2, dist_info.sigma_2));

    return PAGES_TO_BYTES(num_pages);
}

//-----------------------------------------------------------------------------
// Initialize dist info file from command line arguments.
// 
// @param argc number of arguments in argv.
// @param argv array of command line arguments.
// @return On success, returns 0. On error, returns -1.
//-----------------------------------------------------------------------------
int init_dist_info(int argc, char *argv[])
{
    int fd_dist_info;
    char buf[32];

    // Validate that there are enough args.
    if (argc != 6)
    {
        puts("Usage: ./main thresh mu_1 sigma_1 mu_2 sigma_2\n");
        puts("\tthresh  - number of iterations after which to suse the second distribution");
        puts("\tmu_1    - mean of the first distribution");
        puts("\tsigma_1 - standard deviation of the first distribution");
        puts("\tmu_2    - mean of the second distribution");
        puts("\tsigma_2 - standard deviation of the second distribution");
        return -1;
    }

    // Try to open dist info file. Note that we use the following flags:
    // O_CREATE - create file if it doesn't exist
    // O_TRUNC  - truncate file (delete existing contents)
    // O_WRONLY - open in write mode
    if ((fd_dist_info = open(DIST_INFO_FILEPATH, O_CREAT | O_WRONLY | O_TRUNC)) == -1)
    {
        printf("Error: [init_dist_info] failed to open %s\n", DIST_INFO_FILEPATH);
        printf("exiting program...\n");
        return -1;
    }

    sprintf(buf, "%d %d %d %d %d %d",
        0,              // Current iteration (always start at 0).
        atoi(argv[1]),  // Threshold iteration to swtich to using second distribution.
        atoi(argv[2]),  // Mean of first distribution.
        atoi(argv[3]),  // Standard deviation of first distirbution.
        atoi(argv[4]),  // Mean of second distribution.
        atoi(argv[5])); // Standard deviation of second distribution.

    // Try to write string to file.
    if (write(fd_dist_info, buf, strlen(buf)) == -1)
    {
        printf("Error: [init_dist_info] failed to write to %s\n", DIST_INFO_FILEPATH);
        printf("exiting program...");
        close(fd_dist_info);
        return -1;
    }

    // close returns 0 on success and -1 on error.
    return close(fd_dist_info);
}

//-----------------------------------------------------------------------------
// Function called by child process which simply allocates a random ammount of
// memory chosen from a normal distribution.
//-----------------------------------------------------------------------------
void child_proc(void)
{
    // Allocate some random amount of memory.
    char *ptr = (char*) malloc(num_bytes_to_alloc());

    // Sleep for a little bit just to simulate the time that would elapse
    // if the child was to do some work with the memory it allocated.
    usleep(100000);  
    
    if (ptr != NULL)
        free(ptr);

    return; 
}
