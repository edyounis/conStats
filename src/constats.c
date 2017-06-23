/**
 * @File     : constats.c
 * @Author   : Abdullah Younis
 *
 * This library contains statistical functions to help in
 * analyzing and representing data.
 */

#include "constats.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF  9223372036854775807
#define NINF -9223372036854775807

#define ABSOLUTE(x) ((x) >= 0 ? (x) : -(x))
#define DTOC(x)     ((char)('0' + (x)))

/**
 * This function returns the mean of the sample set.
 */
static inline
float constats_get_mean ( int64_t* sample_set, uint64_t sample_size )
{
	register float sum;
	register uint64_t i;

	for ( sum = 0, i = 0; i < sample_size; ++i )
	{
		sum += sample_set[i];
	}

	return sum / (float) sample_size;
}

/**
 * This function returns a tolerance level, the suggested max deviation
 * from the mean before being classified as an outlier.
 */
static inline
int64_t constats_get_tolerance ( int64_t* sample_set, uint64_t sample_size )
{
	register float sum;
	register float mean;
	register uint64_t i;

	// For a quick sketch idea of the data, only go through 1/16 of it.
	if ( sample_size > 16 )
		sample_size >>= 4;

	for ( sum = 0, i = 0; i < sample_size; ++i )
	{
		sum += sample_set[i];
	}

	mean = sum / (float) sample_size;

	for ( sum = 0, i = 0; i < sample_size; ++i )
	{
		sum += ABSOLUTE( sample_set[i] - mean );
	}

	float sketch_abdev = sum / (float) sample_size;

	if ( sketch_abdev > INF / 32 )
		return INF;

	return 5 * sketch_abdev;
}

/**
 * This function populates the stat data structure with statistics.
 */
int constats_calculate_stats ( int64_t* sample_set, uint64_t sample_size, stats_t* stat )
{
	// Error Checking
	if ( stat == NULL || sample_size == 0 )
		return -1;

	stat->N    = sample_size;
	stat->mean = constats_get_mean ( sample_set, sample_size );
	stat->tolerance = constats_get_tolerance ( sample_set, sample_size );

	int64_t upper_thresh = stat->tolerance == INF ? INF : stat->mean + stat->tolerance;
	int64_t lower_thresh = stat->tolerance == INF ? NINF : stat->mean - stat->tolerance;

	// Temporary Values
	stat->min = INF;
	stat->max = NINF;
	stat->norm_min = INF;
	stat->norm_max = NINF;
	stat->outliers = 0;

	register float stdevSum = 0;
	register float abdevSum = 0;
	register float normSum = 0;
	register float normStdevSum = 0;
	register float normAbdevSum = 0;
	register uint64_t i;

	for ( i = 0; i < sample_size; ++i )
	{
		float dev = ABSOLUTE( sample_set[i] - stat->mean );
		abdevSum += dev;
		stdevSum += dev*dev;

		if ( sample_set[i] < stat->min )
			stat->min = sample_set[i];

		if ( sample_set[i] > stat->max )
			stat->max = sample_set[i];

		if ( sample_set[i] > upper_thresh || sample_set[i] < lower_thresh )
		{
			stat->outliers++;
		}
		else
		{
			normSum += sample_set[i];

			if ( sample_set[i] > stat->norm_max )
				stat->norm_max = sample_set[i];

			if ( sample_set[i] < stat->norm_min )
				stat->norm_min = sample_set[i];
		}
	}

	stat->stdev     = sqrt( stdevSum / (float) sample_size );
	stat->abdev     = abdevSum / (float) sample_size;
	stat->norm_mean = normSum / (float) ( sample_size - stat->outliers );

	for ( i = 0; i < sample_size; ++i )
	{
		if ( sample_set[i] <= upper_thresh && sample_set[i] >= lower_thresh )
		{
			float dev = ABSOLUTE( sample_set[i] - stat->norm_mean );
			normAbdevSum += dev;
			normStdevSum += dev*dev;
		}
	}

	stat->norm_stdev = sqrt( normStdevSum / (float) ( sample_size - stat->outliers ) );
	stat->norm_abdev = normAbdevSum / (float) ( sample_size - stat->outliers );

	return 0;
}

/**
 * This function returns the value with the specified zScore.
 */
static inline
uint64_t constats_zrange_value ( stats_t* stat, float zScore )
{
	return stat->norm_mean + zScore*stat->norm_stdev;
}

/**
 * This function returns the zScore with the specified value.
 */
static inline
float constats_zscore_value ( stats_t* stat, int64_t value )
{
	if ( stat->norm_stdev == 0 )
		return 0;

	return (float)(value - stat->norm_mean) / stat->norm_stdev;
}

/**
 * This function counts the number of data points in the zScore range (inclusive).
 */
static inline
uint64_t constats_count_in_range ( int64_t* sample_set, uint64_t sample_size, int64_t min, int64_t max )
{
	register uint64_t i;
	register uint64_t count;

	for ( count = 0, i = 0; i < sample_size; ++i )
		if ( sample_set[i] >= min && sample_set[i] <= max )
			++count;

	return count;
}

/**
 * This function truncates (or pads) the given int64_t into a string with given width
 */
static inline
int constats_truncate ( int64_t value, char* buf, uint64_t width )
{
	// Error Checking
	if ( width == 0 )
		return -1;

	// Negative Symbol
	if ( value < 0 )
	{
		buf[0] = '-';
		return constats_truncate ( -value, ++buf, --width );
	}

	uint64_t buf_index   = 0;
	uint64_t digit_index = 0;

	char digits[21];
	memset( digits, 0, 21 );

	// Turn value into char array
	while ( value > 0 && digit_index < 20 )
	{
		int64_t digit = value % 10;
		value = value / 10;
		digits[digit_index++] = DTOC(digit);
	}

	// Reverse it into buf
	while ( width > 0 && digit_index >= 1 )
	{
		buf[buf_index++] = digits[--digit_index];
		--width;
	}

	// Append suffix, if needed
	if ( digit_index >= 1 )
	{
		int chars_deleted = 3 - (digit_index % 3);

		buf_index   -= chars_deleted;
		width       += chars_deleted;
		digit_index += chars_deleted;

		if ( buf_index < 0 )
			return -1;

		char suffix = ' ';

		if ( digit_index >= 18 )
			suffix = 'E';
		else if ( digit_index >= 15 )
			suffix = 'P';
		else if ( digit_index >= 12 )
			suffix = 'T';
		else if ( digit_index >= 9 )
			suffix = 'G';
		else if ( digit_index >= 6 )
			suffix = 'M';
		else if ( digit_index >= 3 )
			suffix = 'K';

		buf[buf_index++] = suffix;
		width--;
	}

	// Pad with spaces
	while ( width > 0 )
	{
		buf[buf_index++] = ' ';
		--width;
	}

	return 0;
}

/**
 * This function prints one bar in a histogram corresponding to the range given by zScoreMin and zScoreMax.
 */
static inline
int constats_print_zrange_bar ( int64_t* sample_set, uint64_t sample_size, stats_t* stat, float zScoreMin, float zScoreMax )
{
	char bar[33];
	char count_str[13];
	char value_below_str[14];
	char value_above_str[14];

	bar[32]             = '\0';
	count_str[12]       = '\0';
	value_below_str[13] = '\0';
	value_above_str[13] = '\0';

	int64_t value_below = constats_zrange_value( stat, zScoreMin );
	int64_t value_above = constats_zrange_value( stat, zScoreMax );

	// Calculate the number of 'X's
	uint64_t count   = constats_count_in_range( sample_set, sample_size, value_below, value_above );
	uint64_t X_value = sample_size >> 5 == 0 ? 1 : sample_size >> 5;
	uint64_t X_count = count / X_value;

	// Construct the bar string
	memset( bar, 'X', X_count );
	memset( bar + X_count, ' ', 32 - X_count );

	constats_truncate( count, count_str, 12 );
	constats_truncate( value_below, value_below_str, 13 );
	constats_truncate( value_above, value_above_str, 13 );


	printf ( "%s -> %s : %s : %s\n", value_below_str, value_above_str, bar, count_str );
	return 0;
}

/**
 * This function prints a histogram of the data.
 */
static inline
int constats_print_zhistogram ( int64_t* sample_set, uint64_t sample_size, stats_t* stat )
{
	// print zranges from max(-3, min) to min(3, max)
	float i = constats_zscore_value( stat, stat->min );
	i = i < -3 ? -3 : i; // i = max(-3, min)

	float maxZ = constats_zscore_value( stat, stat->max );
	maxZ = maxZ > 3 ? 3 : maxZ; // maxZ = min(3, max)

	if ( i >= maxZ )
		constats_print_zrange_bar( sample_set, sample_size, stat, -0.5, 0.5 );

	for ( ; i < maxZ; i += 0.5 )
		constats_print_zrange_bar( sample_set, sample_size, stat, i, i + 0.5 );
	
	//constats_print_zrange_bar( sample_set, sample_size, stat, constats_zscore_value( stat, stat->min ), i );
	
	// float maxZScore = constats_zscore_value( stat, stat->max );
	// for ( ; i < maxZScore && i < 3; i += 0.5 )
	// {
	// 	constats_print_zrange_bar( sample_set, sample_size, stat, i, ( i+0.5 <= maxZScore ? i+0.5 : maxZScore ) );
	// }

	return 0;
}

/**
 * This function prints statistics of the sample set to stdout.
 */
int constats_print_stats ( int64_t* sample_set, uint64_t sample_size, stats_t* stat )
{
	printf ( "-------------------------------------------------------------------------------\n" );
	printf ( "Sample Size            : %lu\n", stat->N );
	printf ( "Average value          : %f\n", stat->mean );
	printf ( "Minimum value          : %ld\n", stat->min );
	printf ( "Maximum value          : %ld\n", stat->max );
	printf ( "Standard Deviation     : %f\n", stat->stdev );
	printf ( "Mean Absolute Deviation: %f\n", stat->abdev );
	printf ( "\n" );
	printf ( "Outlier Count   : %lu\n", stat->outliers );
	if ( stat->outliers > 0 )
	{
		printf ( "Without Outliers:\n");
		printf ( "\tAverage value          : %f\n", stat->norm_mean );
		printf ( "\tMinimum value          : %ld\n", stat->norm_min );
		printf ( "\tMaximum value          : %ld\n", stat->norm_max );
		printf ( "\tStandard Deviation     : %f\n", stat->norm_stdev );
		printf ( "\tMean Absolute Deviation: %f\n", stat->norm_abdev );
	}
	printf ( "\n" );
	constats_print_zhistogram( sample_set, sample_size, stat );
	printf ( "\n" );
	printf ( "Summary:\nnorm mean:\t%f;\tnorm abs dev:\t%f\nmin:\t\t%lu;\t\tmax:\t\t%lu\n", 
				stat->norm_mean, stat->norm_abdev, stat->min, stat->max );
	printf ( "-------------------------------------------------------------------------------\n" );

	return 0;
}

/**
 * This function calculates and prints statistics of the given sample set.
 */
int constats_get_and_print_stats ( int64_t* sample_set, uint64_t sample_size )
{
	int error_code = 0;

	stats_t stats;
	error_code = constats_calculate_stats ( sample_set, sample_size, &stats );

	if ( error_code != 0 )
		return error_code;

	return constats_print_stats ( sample_set, sample_size, &stats );
}