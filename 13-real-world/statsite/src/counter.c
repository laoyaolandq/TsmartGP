#include <math.h>
#include "counter.h"

/**
 * Initializes the counter struct
 * @arg counter The counter struct to initialize
 * @return 0 on success.
 */
int init_counter(counter *counter) {
    counter->sum = 0;
    counter->count = 0;
    return 0;
}

/**
 * Adds a new sample to the struct
 * @arg counter The counter to add to
 * @arg sample The new sample value
 * @return 0 on success.
 */
int counter_add_sample(counter *counter, double sample, double sample_rate) {
    counter->sum += sample;
    counter->count += 1 / sample_rate;
    return 0;
}

/**
 * Returns the number of samples in the counter
 * @arg counter The counter to query
 * @return The number of samples
 */
uint64_t counter_count(counter *counter) {
    return counter->count;
}

/**
 * Returns the sum of the counter
 * @arg counter The counter to query
 * @return The sum of values
 */
double counter_sum(counter *counter) {
    return counter->sum;
}
