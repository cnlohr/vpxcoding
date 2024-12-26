#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define VPXCODING_DECORATOR
#define VPXCODING_READER
#define VPXCODING_WRITER
#define VPXCODING_IMPLEMENTATION
#include "vpxcoding.h"

uint8_t dummydata[1024*64];
uint8_t bufferO[1024*1024*1024];
int main( int argc, char ** argv )
{
	srand( 1 );
	int i;
	printf( "Probability of 1," );
	for( int probability = 0; probability < 256; probability++ )
	{
		printf( "%d,", probability );
	}
	printf( "Best\n" );	double percentones = 0.0;
	for( percentones = 0.0; percentones < 100; percentones+=0.1 )
	{
		memset( dummydata, 0, sizeof( dummydata ) );
		for( i = 0; i < sizeof(dummydata); i++ )
		{
			int b;
			for( b = 0; b < 8; b++ )
			{
				int bit = 
					//((i*8+b)) < (percentones/100.0 * (sizeof(dummydata)*8));
					(rand()%100000) < percentones * 1000;
				dummydata[i] |= bit << b;
			}
		}

		printf( "%.1f%%,",percentones);
		int bestprob;
		double bestrate;
		bestprob = 0;
		bestrate = 1e10;
		for( int probability = 0; probability < 256; probability++ )
		{
			vpx_writer w = { 0 };
			vpx_start_encode( &w, bufferO, sizeof(bufferO));
			for( i = 0; i < sizeof(dummydata); i++ )
			{
				int data = dummydata[i];
				int bits = 8;
				int bit;
				for (bit = bits - 1; bit >= 0; bit--)
				{
					int outbit = (data >> bit) & 1;
					vpx_write(&w, outbit, probability);
				}
			}
			vpx_stop_encode(&w);

			if( w.error )
			{
				fprintf( stderr, "Encoder reported %d\n", w.error );
				return -10;
			}
			//printf( "Relative Size: %.2f %%\n", w.pos * 100.0 / sizeof(dummydata) );
			double rate = w.pos * 100.0 / sizeof(dummydata);
			printf( "%.4f,", rate );
			if( bestrate > rate )
			{
				bestrate = rate;
				bestprob = probability;
			}
			fflush( stdout );
			vpx_reader reader;
			int ret = vpx_reader_init(&reader, bufferO, w.pos, 0, 0 );
			for( i = 0; i < sizeof(dummydata); i++ )
			{
				int bits = 8;
				int bit;
				uint8_t data = 0;
				for (bit = bits - 1; bit >= 0; bit--)
					data = (data<<1) | vpx_read(&reader, probability);
				if( data != dummydata[i] )
				{
					fprintf( stderr, "Disagree at byte %d (%08x != %08x) (probability %d)\n", i, data, dummydata[i], probability );
					return -9;
				}
			}
		}
		printf( "%d\n",bestprob );
	}


	return 0;
}
