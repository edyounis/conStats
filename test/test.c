#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "../include/constats.h"

# define TRANSACTIONS 100000

#if RAND_MAX/256 >= 0xFFFFFFFFFFFFFF
  #define LOOP_COUNT 1
#elif RAND_MAX/256 >= 0xFFFFFF
  #define LOOP_COUNT 2
#elif RAND_MAX/256 >= 0x3FFFF
  #define LOOP_COUNT 3
#elif RAND_MAX/256 >= 0x1FF
  #define LOOP_COUNT 4
#else
  #define LOOP_COUNT 5
#endif

uint64_t rand_uint64(void) {
  uint64_t r = 0;
  for (int i=LOOP_COUNT; i > 0; i--) {
    r = r*(RAND_MAX + (uint64_t)1) + rand();
  }
  return r;
}

int main ( void )
{
	srand( time( NULL ) );

	//int64_t random_int[10] = { -6, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	int64_t* random_int = (int64_t*) malloc ( TRANSACTIONS * sizeof(int64_t) );
	uint64_t  trans_id;

	for ( trans_id = 0; trans_id < TRANSACTIONS; ++trans_id )
	{
		random_int[trans_id] = (int64_t)rand();
		//printf("%ld\n", random_int[trans_id]);
	}

	constats_get_and_print_stats( random_int, TRANSACTIONS );

	return 0;
}