#include <stdlib.h>
#include <math.h>
#include "../include/classifier.h"


//-----------------------------------------------------------------------------
// Data needed by the Gaussian classifier to determine if a sample is
// within the class or outside the class.
//-----------------------------------------------------------------------------
struct gaussian_occ_data_t
{
    double mean;
    double stddev;
};

//-----------------------------------------------------------------------------
// Trains the one class classifier on a training set.
// 
// @param classifier the classifier object to train.
// @param samples the training feature set containing only data points of a single class.
// @param num_samples number of data points in the samples array.
//-----------------------------------------------------------------------------
static void train(struct gaussian_occ_t *classifier, double samples[], size_t num_sample)
{
    double mean = 0;
    double stddev = 0;
    // Compute smaple mean
    for (int i = 0; i < num_sample; ++i)
    {
        mean += samples[i];
    }
    mean /= num_sample;

    // Compute sample standard deviation
    for (int i = 0; i < num_sample; ++i)
    {
        stddev += (samples[i] - mean) * (samples[i] - mean);
    }
    stddev /= num_sample;
    stddev = sqrt(stddev);

    // Store trained values in classifer
    classifier->data->mean = mean;
    classifier->data->stddev = stddev;
}

//-----------------------------------------------------------------------------
// Predict whether a sample is within the class on which the one class
// classifier has been trainined on.
// 
// @param classifier the trianined classifier object.
// @param sample the sample to classify.
// @return 1 if sample is within the class, 0 otherwise. 
//-----------------------------------------------------------------------------
static int classify(struct gaussian_occ_t *classifier, double sample)
{
    double z_score = (sample - classifier->data->mean) / classifier->data->stddev;
    return z_score > 3 ? 0 : 1;
}

//-----------------------------------------------------------------------------
// Create a new gaussian one class classifier object.
//-----------------------------------------------------------------------------
struct gaussian_occ_t *create_classifier(void)
{
    // Allocate memory for classifier struct.
    struct gaussian_occ_t *classifier = (struct gaussian_occ_t *) malloc(sizeof(struct gaussian_occ_t));
    
    if (classifier == NULL)
        return NULL;

    // Allocate memory for classifier private data.
    classifier->data = (struct gaussian_occ_data_t *) malloc(sizeof(struct gaussian_occ_data_t));

    if (classifier->data == NULL)
    {
        delete_classifier(classifier);
        return NULL;
    }

    // Attach public methods.
    classifier->train = &train;
    classifier->classify = &classify;

    return classifier;
}

//-----------------------------------------------------------------------------
// Free up the resources allocated for a gaussian one class classifier object.
//-----------------------------------------------------------------------------
void delete_classifier(struct gaussian_occ_t *classifier)
{
    if (classifier != NULL)
    {
        // Free data first because if we free classifier first, we'll lose
        // pointer to the data struct.
        if (classifier->data != NULL)
        {
            free(classifier->data);
        }
        free(classifier);
    }
}