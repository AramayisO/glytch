#include <stdio.h>
#include <stdlib.h>
#include "../include/stats_util.h"

//-----------------------------------------------------------------------------
// Private data members of stats_t object.
//-----------------------------------------------------------------------------
struct stats_data_t
{
    double tp; // number of true positives
    double tn; // number of true negatives
    double fp; // number of false positives 
    double fn; // number of false negatives
};

//-----------------------------------------------------------------------------
// Add statistic to record.
// 
// @param stats the stats object.
// @param actual the actual class.
// @param predicted the predicted class.
//-----------------------------------------------------------------------------
static void add_stat(struct stats_t *stats, int actual, int predicted)
{
    if ((actual == 1) && (predicted == 1))
        stats->data->tp++;
    else if ((actual == 1) && (predicted == 0))
        stats->data->fn++;
    else if ((actual == 0) && (predicted == 1))
        stats->data->fp++;
    else
        stats->data->tn++;
}

//-----------------------------------------------------------------------------
// Prints the Confusion Matrix base on the current statistics.
//----------------------------------------------------------------------------- 
static double compute_accuracy(struct stats_t *stats)
{
    return (stats->data->tp + stats->data->tn) / 
        (stats->data->tp + stats->data->tn + stats->data->fp + stats->data->fn);
}

//-----------------------------------------------------------------------------
// Compute the accuracy base on the current statistics.
//-----------------------------------------------------------------------------
static double compute_recall(struct stats_t *stats)
{
    return stats->data->tp / (stats->data->tp + stats->data->fn);
}

//-----------------------------------------------------------------------------
// Compute the recall based on the current statistics.
//-----------------------------------------------------------------------------
static double compute_precision(struct stats_t *stats)
{
    return stats->data->tp / (stats->data->tp + stats->data->fp);
}

//-----------------------------------------------------------------------------
// Compute the precision based on the current statistics.
//-----------------------------------------------------------------------------
static double compute_f1_score(struct stats_t *stats)
{
    double precision = stats->get_precision(stats);
    double recall    = stats->get_recall(stats);
    return 2 * (precision * recall) / (precision + recall);
}

//-----------------------------------------------------------------------------
// Compute the F1-score based on the current statistics.
//-----------------------------------------------------------------------------
static void print_confusion_matrix(struct stats_t *stats)
{
    printf("           +-------------------------------------+\n");
    printf("           |                Actual               |\n");
    printf("           |------------------+------------------|\n"); 
    printf("           |         D1       |         D2       |\n");
    printf("+---+------+------------------+------------------|\n");
    printf("|   |      |                  |                  |\n");
    printf("| P |      |                  |                  |\n");
    printf("| r |  D1  | % 16.8f | % 16.8f |\n", stats->data->tp, stats->data->fp);
    printf("| e |      |                  |                  |\n");
    printf("| d |      |                  |                  |\n");
    printf("| i |------+------------------+------------------|\n");
    printf("| c |      |                  |                  |\n");
    printf("| t |      |                  |                  |\n");
    printf("| e |  D2  | % 16.8f | % 16.8f |\n", stats->data->fn, stats->data->tn);
    printf("| d |      |                  |                  |\n");
    printf("|   |      |                  |                  |\n");
    printf("+---+------+------------------+------------------+\n");
}

//-----------------------------------------------------------------------------
// Allocate and initialize stats_t object.
//-----------------------------------------------------------------------------
struct stats_t *create_stats(void)
{
    // Allocate memory.
    struct stats_t *stats = (struct stats_t *) malloc(sizeof(struct stats_t));

    if (stats == NULL)
        return NULL;

    stats->data = (struct stats_data_t *) malloc(sizeof(struct stats_data_t));

    if (stats->data == NULL)
    {
        delete_stats(stats);
        return NULL;
    }

    // Initialize state
    stats->data->tp = 0;
    stats->data->tn = 0;
    stats->data->fp = 0;
    stats->data->fn = 0;

    // Attach member functions
    stats->add_stat               = &add_stat;
    stats->print_confusion_matrix = &print_confusion_matrix;
    stats->get_accuracy           = &compute_accuracy;
    stats->get_recall             = &compute_recall;
    stats->get_precision          = &compute_precision;
    stats->get_f1_score           = &compute_f1_score;

    return stats;
}

//-----------------------------------------------------------------------------
// Deallocate dynamically allocated resources for stats_t object.
//-----------------------------------------------------------------------------
void delete_stats(struct stats_t *stats)
{
    if (stats != NULL)
    {
        if (stats->data != NULL)
        {
            free(stats->data);
        }
        free(stats);
    }
}