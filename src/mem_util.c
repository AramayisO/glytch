#include <stdio.h>
#include <stdlib.h>
#include "../include/mem_util.h"

//-----------------------------------------------------------------------------
// Parses /proc/[pid]/statm file and updates the fields of statm with 
// the current memory usage data of process pid.
// 
// @param pid process for which to parse memory usage data.
// @param statm on success, will be updated with the current memory usage of the process.
// @return If the file is parsed successfully, return 0. Otherwise, returns -1. 
//-----------------------------------------------------------------------------
int parse_statm(pid_t pid, struct statm_t *statm)
{    
    char filepath[32];
    sprintf(filepath, "/proc/%d/statm", (int) pid);
    FILE *pstatm = fopen(filepath, "r");
    
    if (pstatm == NULL)
    {
        return -1;
    }

    fscanf(pstatm, "%lu %lu %lu %lu %lu %lu %lu",
        &statm->size,
        &statm->resident,
        &statm->shared,
        &statm->text,
        &statm->lib,
        &statm->data,
        &statm->dt);

    fclose(pstatm);
    return 0;
}
