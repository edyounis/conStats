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

/**
 * This function prints statistics of the sample set splits to stdin.
 */
extern int constats_print_info_split ( uint64_t* sample_set, uint64_t sample_size );

/**
 * This function prints statistics of the sample set to stdin.
 */
extern int constats_print_info ( uint64_t* sample_set, uint64_t sample_size );

#endif