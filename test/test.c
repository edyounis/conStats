#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "../include/constats.h"

# define TRANSACTIONS 1e7

int main ( void )
{
	srand( time( NULL ) );

	uint64_t* random_int = (uint64_t*) malloc ( TRANSACTIONS * sizeof(uint64_t) );
	uint64_t  trans_id;

	for ( trans_id = 0; trans_id < TRANSACTIONS; ++trans_id )
	{
		random_int[trans_id] = (uint64_t)rand();
	}

	constats_print_info( random_int, TRANSACTIONS );

	return 0;
}
