#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <stddef.h>

/**
 * Private data used by the classifier. Forward declared here so it can
 * be used in the classifier struct, but the implementation is private.
 */
struct gaussian_occ_data_t;

/**
 * Gaussian one class classifier
 */
struct gaussian_occ_t
{
    /**
     * Private data used by the classifier.
     */
    struct gaussian_occ_data_t *data;

    /**
     * Trains the one class classifier on a training set.
     * 
     * @param self the classifier object to train.
     * @param samples the training feature set containing only data points of a single class.
     * @param num_samples number of data points in the samples array.
     */
    void (*train)(struct gaussian_occ_t *self, double samples[], size_t num_samples);

    /**
     * Predict whether a sample is within the class on which the one class
     * classifier has been trainined on.
     * 
     * @param self the trianined classifier object.
     * @param sample the sample to classify.
     * @return 1 if sample is within the class, 0 otherwise. 
     */
    int (*classify)(struct gaussian_occ_t *self, double sample);
};

/**
 * Create a new gaussian one class classifier object.
 */
struct gaussian_occ_t *create_classifier(void);

/**
 * Free up the resources allocated for a gaussian one class classifier object.
 */
void delete_classifier(struct gaussian_occ_t *classifier);

#endif