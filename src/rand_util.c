#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../include/rand_util.h"

//-----------------------------------------------------------------------------
// Generates a random number from the standard normal distribution with 
// mean 0 and standard deviation 1. The implementation is the Marsaglia
// polar method. See https://en.wikipedia.org/wiki/Marsaglia_polar_method 
// for more info on the Marsaglia polar method.
//
// TODO: Implement the Ziggurat algorithm for pseudo random number sampling
// instead, which is much faster than the Marsaglia method 97% of the time.
//-----------------------------------------------------------------------------
static double std_norm_rand(void)
{
    // We can't just seed rand once in main because rand will be called from
    // different processes. We need to seed rand for each process. Seeding
    // with time(0) is not suitable since it returns seconds since epoch.
    // Seconds resolution is too low and multiple processes can run and call
    // this function in that time. Thus, we use nanoseconds resolution.
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME , &tp);
    srand(tp.tv_nsec);

    double x, y, s;
    
    do
    {
        x = 2 * ((double) rand() / (double) RAND_MAX) - 1.0;
        y = 2 * ((double) rand() / (double) RAND_MAX) - 1.0;
        s = (x * x) + (y * y);
    }
    while ((s <= 0) || (s >= 1.0));

    double x_sample = x * sqrt(-2 * log(s) / s);
    // double y_sample = y * sqrt(-2 * log(s) / s);

    return x_sample; 
}

//-----------------------------------------------------------------------------
// Generates a random number from the normal distribution with mean mu and
// standard deviation sigma.
//
// @param mu the mean of the normal distribution to be sampled.
// @param sigma the standard deviation of the normal distribution to be sampled
//-----------------------------------------------------------------------------
double norm_rand(double mu, double sigma)
{
    // Given a normal distribution N(mu_0, sigma_0), we can apply a linear
    // transformation on it to create a new normal distribution N(mu, sigma).
    // The transformation is defined by the system of 2 equations
    // 
    //      a   * mu_0 + b  = mu
    //      a^2 * sigma_0^2 = sigma^2
    // 
    // Solving the system for the parameters a and b gives a = sigma / sigma_0
    // and b = mu - mu_0 * (sigma / sigma_0). 
    //
    // If we start with a sample from the standard normal distribution N(0, 1),
    // we get a = sigma and b = mu. So we get the linear transformation
    //
    //      N(mu, sigma) = sigma * N(0, 1) + mu
    return sigma * std_norm_rand() + mu;
}

