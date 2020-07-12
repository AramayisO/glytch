#ifndef STATS_UTIL_H
#define STATS_UTIL_H

#include <stddef.h>

/**
 * Private data members of a stats_t object.
 * Forward declared here to make compiler happy.
 */
struct stats_data_t;

/**
 * Struct with data and methods for computing statistics about data samples.
 */
struct stats_t
{
    struct stats_data_t *data;

    /**
     * Add statistic to record.
     * 
     * @param self the stats object.
     * @param actual the actual class.
     * @param predicted the predicted class.
     */
    void (*add_stat)(struct stats_t *self, int actual, int predicted);

    /**
     * Prints the Confusion Matrix base on the current statistics.
     */ 
    void (*print_confusion_matrix)(struct stats_t *self);

    /**
     * Compute the accuracy base on the current statistics.
     */
    double (*get_accuracy)(struct stats_t *self);

    /**
     * Compute the recall based on the current statistics.
     */
    double (*get_recall)(struct stats_t *self);

    /**
     * Compute the precision based on the current statistics.
     */
    double (*get_precision)(struct stats_t *self);

    /**
     * Compute the F1-score based on the current statistics.
     */
    double (*get_f1_score)(struct stats_t *self);
};

/**
 * Allocate and initialize stats_t object.
 */
struct stats_t *create_stats(void);

/**
 * Deallocate dynamically allocated resources for stats_t object.
 */
void delete_stats(struct stats_t *stats);

#endif