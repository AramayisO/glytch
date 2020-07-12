#ifndef MEM_UTIL_H
#define MEM_UTIL_H

#include <sys/types.h>

/**
 * Converts n kilobytes to bytes. 
 */
#define KB_TO_BYTES(n) (n * 1024) 

/**
 * Number of bytes per page.
 */
#define PAGE_SIZE (4096)

/**
 * Converts n pages to size in bytes. 
 */
#define PAGES_TO_BYTES(n) (n * PAGE_SIZE) 

/**
 * Structure representing the data provided in /proc/[pid]/statm, which 
 * provides information about memory usage measured in pages. The columns
 * of /proc/[pid]/statm are representded by the fields of this struct.
 * See `man proc` for more details.
 */
struct statm_t {
    unsigned long size;     // total program size 
    unsigned long resident; // resident set size 
    unsigned long shared;   // number of resident shared pages 
    unsigned long text;     // size of text (code)
    unsigned long lib;      // library (unused since Linux 2.6; always 0)
    unsigned long data;     // size of data + stack
    unsigned long dt;       // number of dirty pages (unused since Linux 2.6; always 0)
};

/**
 * Parses /proc/[pid]/statm file and updates the fields of statm with 
 * the current memory usage data of process pid.
 * 
 * @param pid process for which to parse memory usage data.
 * @param statm on success, will be updated with the current memory usage of the process.
 * @return If the file is parsed successfully, return 0. Otherwise, returns -1. 
 */
int parse_statm(pid_t pid, struct statm_t *statm);

#endif
