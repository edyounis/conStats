/**
 * @File     : constats.h
 * @Author   : Abdullah Younis
 *
 * This library contains statistical functions to help in
 * analyzing and representing data.
 */

#ifndef CONSTATS_LIB_LOCK
#define CONSTATS_LIB_LOCK

#include <stdint.h>

typedef struct stats_t
{
	uint64_t N;			// The size of the sample set

	double mean;		// The mean of the data
	double stdev;		// The standard deviation
	double abdev;		// The mean absolute deviation
	int64_t min;		// The minimum value
	int64_t max;		// The maximum value

	int64_t tolerance;	// The tolerance level used in classifying outliers
	uint64_t outliers;	// The number of data points classified as outliers

	double norm_mean;	// The mean of the data without the outliers
	double norm_stdev;	// The standard deviation without the outliers
	double norm_abdev;	// The mean absolute deviation without the outliers
	int64_t norm_min;	// The minimum value without the outliers
	int64_t norm_max;	// The maximum value without the outliers

} stats_t;

/**
 * This function populates the stat data structure with statistics.
 */
extern int constats_calculate_stats ( int64_t* sample_set, uint64_t sample_size, stats_t* stat );

/**
 * This function prints statistics of the sample set to stdout.
 */
extern int constats_print_stats ( int64_t* sample_set, uint64_t sample_size, stats_t* stat );
 
/**
 * This function calculates and prints statistics of the given sample set.
 */
extern int constats_get_and_print_stats ( int64_t* sample_set, uint64_t sample_size );

#endif