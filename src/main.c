#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/child_proc.h"
#include "../include/mem_util.h"
#include "../include/classifier.h"
#include "../include/stats_util.h"

//=============================================================================
// CONSTANTS:
//=============================================================================
#define MEM_DATA_FILEPATH    "data/mem.data"
#define KST_FULL_PATH        "/usr/bin/kst2"
#define KST_CMD              "kst2"
#define KST_XLABEL           "Child Proccess"
#define KST_YLABEL_P1        "Mem. Usage (pages)"
#define KST_YLABLE_P2        "Classifier Output"
#define KST_X_COL            "1"
#define KST_P1_Y_COL         "2"
#define KST_P2_Y_COL         "3"
#define D1_SAMPLES_START     250            
#define D2_SAMPLES_START     500
#define TOTAL_NUM_SAMPLES    1000

//=============================================================================
// MAIN:
//=============================================================================
int main(int argc, char *argv[])
{  
    int                    fd_mem_data;    // File descriptor for memory usage output file
    int                    wstatus;        // Wait status of child processes
    int                    prediction;     // Classifier prediction 1 = D1, 0 = D2
    char                   buf[32];        // Miscelaneous use like writing data to files
    pid_t                  pid;            // Process ID of child processes
    double                *train_samples;  // Array of samples used to train the classifier
    unsigned long          base_mem_usage; // Baseline memory usage of parent process
    unsigned long          mem_usage;      // Used in computing memory usage of child processes
    struct statm_t         statm;          // Struct that stores data from /proc/[pid]/statm file
    struct stats_t        *stats;          // Object for working with statistics
    struct gaussian_occ_t *classifier;     // Gaussian one class classifier

    //-------------------------------------------------------------------------
    // Initialize file containing information regarding the distirbutions D1
    // and D2 that child processes will use from the command line arguments.
    //-------------------------------------------------------------------------
    if (init_dist_info(argc, argv) == -1)
    {
        exit(EXIT_FAILURE);
    }

    //-------------------------------------------------------------------------
    // Open file for writing memory usage data to use for analysis and plotting.
    // Create file if it doesn't already exists. If the file already exists,
    // then delete its contents.
    //-------------------------------------------------------------------------
    if ((fd_mem_data = open(MEM_DATA_FILEPATH, O_CREAT | O_TRUNC | O_WRONLY)) == -1)
    {
        printf("Error: unable to open %s\n", MEM_DATA_FILEPATH);
        exit(EXIT_FAILURE);
    }

    //-------------------------------------------------------------------------
    // Create a new process to run KST Plot in the background and plot the 
    // memory usage data in real time.
    //-------------------------------------------------------------------------
    pid = fork();

    // An error occured
    if (pid < 0)
    {
        printf("Error: unable to fork process.");
        close(fd_mem_data);
        exit(EXIT_FAILURE);
    }
    // Child process
    else if (pid == 0)
    {
        execl(KST_FULL_PATH, KST_CMD, MEM_DATA_FILEPATH, 
            "--xlabel", KST_XLABEL,          "-x", KST_X_COL, 
            "--ylabel", KST_YLABEL_P1,       "-y", KST_P1_Y_COL, 
            "--ylabel", KST_YLABLE_P2, "-d", "-y", KST_P2_Y_COL, 
            NULL);
                
        exit(EXIT_SUCCESS);
    }
    
    // Parent process

    //-------------------------------------------------------------------------
    // Get baseline memory usage of parent process. When we fork child process,
    // they will be copies of the parent process memory space, so they will
    // start out with as much memory pages allocated as the parent had at the
    // time of the fork. We will subtract this amount from the memory usage 
    // of each child process to get a more accurate reading.
    //-------------------------------------------------------------------------
    parse_statm(getpid(), &statm);
    base_mem_usage = statm.data;
    
    //-------------------------------------------------------------------------
    // Initialize all the object and memory that will be needed.
    //-------------------------------------------------------------------------
    stats         = create_stats();
    classifier    = create_classifier();
    train_samples = (double*) malloc(sizeof(double) * 1000);

    if ((classifier == NULL) || (train_samples == NULL) || (stats == NULL))
    {
        printf("Error: unable to allocate enough memory\n");
        delete_stats(stats);
        delete_classifier(classifier);
        close(fd_mem_data);    
        if (train_samples != NULL)
            free(train_samples);
        exit(EXIT_FAILURE);
    }

    //-------------------------------------------------------------------------
    // Create child processes and monitor their memory usage.
    //-------------------------------------------------------------------------
    for (int iter = 0; iter < TOTAL_NUM_SAMPLES; iter++)
    {
        pid = fork();

        // Error occured
        if (pid < 0)
        {
            printf("Error: unable to fork process.");
            delete_stats(stats);
            delete_classifier(classifier);
            free(train_samples);
            close(fd_mem_data);    
            exit(EXIT_FAILURE);
        }
        // Child process
        else if (pid == 0)
        {
            child_proc();
            exit(EXIT_SUCCESS);
        }

        // Parent process

        //---------------------------------------------------------------------
        // Determine memory usage of child process
        //---------------------------------------------------------------------
        mem_usage = 0;

        while(!waitpid(pid, &wstatus, WNOHANG))
        {
            parse_statm(pid, &statm);
            if (mem_usage < statm.data)
            {
                mem_usage = statm.data;
            }
        }
        
        // Correct for baseline memory usage of parent process
        mem_usage -= base_mem_usage;
        
        //---------------------------------------------------------------------
        // Train the gaussian one class classifier.
        //---------------------------------------------------------------------
        if (iter < D1_SAMPLES_START)
        {
            train_samples[iter] = mem_usage;
            prediction = 1;
            if (iter == (D1_SAMPLES_START - 1))
                classifier->train(classifier, train_samples, D1_SAMPLES_START);
        }
        //---------------------------------------------------------------------
        // Classify samples: in this case, all samples are from distribution D1
        //---------------------------------------------------------------------
        else if ((iter >= D1_SAMPLES_START) && (iter < D2_SAMPLES_START))
        {
            prediction = classifier->classify(classifier, (double) mem_usage);
            stats->add_stat(stats, 1, prediction);
        }
        //---------------------------------------------------------------------
        // Classify samples: in this case, all samples are from distribution D2
        //---------------------------------------------------------------------
        else
        {
            prediction = classifier->classify(classifier, (double) mem_usage);
            stats->add_stat(stats, 0, prediction);
        }

        //---------------------------------------------------------------------
        // Write data to file so it can be parsed for real time plotting and
        // also for later analysis.
        //---------------------------------------------------------------------
        sprintf(buf, "%d %lu %d\n", iter, mem_usage, prediction);
        if (write(fd_mem_data, buf, strlen(buf)) == -1)
        {
            printf("[main] Error: unable to write to %s\n", MEM_DATA_FILEPATH);
            printf("exiting program...\n");
            delete_stats(stats);
            delete_classifier(classifier);
            free(train_samples);
            close(fd_mem_data);    
            exit(EXIT_FAILURE);
        }
    }

    //-------------------------------------------------------------------------
    // Print out statistics
    //-------------------------------------------------------------------------    
    stats->print_confusion_matrix(stats);
    printf("Accuracy  = %0.8f\n", stats->get_accuracy(stats));
    printf("Recall    = %0.8f\n", stats->get_recall(stats));
    printf("Precision = %0.8f\n", stats->get_precision(stats));
    printf("F1-score  = %0.8f\n", stats->get_f1_score(stats));

    //-------------------------------------------------------------------------
    // Clean up
    //-------------------------------------------------------------------------
    delete_stats(stats);
    delete_classifier(classifier);
    free(train_samples);
    close(fd_mem_data);    

    return 0;
}