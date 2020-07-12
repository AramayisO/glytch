#ifndef RAND_UTIL_H
#define RAND_UTIL_H

/**
 * Generates a random number from the normal distribution with mean mu and
 * standard deviation sigma.
 *
 * @param mu the mean of the normal distribution to be sampled.
 * @param sigma the standard deviation of the normal distribution to be sampled
 */
double norm_rand(double mu, double sigma);

#endif
