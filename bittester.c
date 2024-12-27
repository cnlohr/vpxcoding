#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define VPXCODING_READER
#define VPXCODING_WRITER
#define VPXCODING_NOTABLE
#include "vpxcoding.h"

double GetAbsoluteTime()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return ((double)tv.tv_usec)/1000000. + (tv.tv_sec);
}

uint8_t dummydata[1024*1024*16];

//#define TEST_RANDOM_PROBS

#ifndef TEST_RANDOM_PROBS
const int arbitrary_prob = 187;
#else
uint8_t dummyprobs[1024*1024*16*8];
#endif

int main( int argc, char ** argv )
{
	int i;
#if 0
	for( i = 0; i < 256; i++ )
	{
		printf( "%2d", VPXCODING_VPXNORM( i ) );
		if( ( i & 15 ) == 15 )
		{
			printf( "\n" );
		}
	}
	return 0;
#endif
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		//dummydata[i] = rand();
		dummydata[i] = ((rand()&0xff)*(rand()&0xff)) >> 10;
	}

	vpx_writer w = { 0 };
	uint8_t * bufferO = malloc(sizeof(dummydata)*20);
	vpx_start_encode( &w, bufferO, sizeof(dummydata)*20);

	double startEnc = GetAbsoluteTime();
	int bitno = 0;
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		int data = dummydata[i];
		int bits = 8;
		int bit;
		for (bit = bits - 1; bit >= 0; bit--)
		{
#ifdef TEST_RANDOM_PROBS
			int prob = dummyprobs[bitno++];
#else
			int prob = arbitrary_prob;
#endif
			int outbit = (data >> bit) & 1;
			vpx_write(&w, outbit, prob);
		}
	}
	vpx_stop_encode(&w);
	double endEnc = GetAbsoluteTime();

	printf( "Input  Len: %ld bytes (%ld bits)\n", sizeof(dummydata), sizeof(dummydata) * 8 );
	printf( "Output Len: %d (%d count) (Out of a maximum of %d bytes)\n", w.pos, w.count, w.size );
	if( w.error )
	{
		fprintf( stderr, "Encoder reported %d\n", w.error );
		return -10;
	}
	printf( "Error / Range: %d %d\n", w.error, w.range );
	printf( "Relative Size: %.2f %%\n", w.pos * 100.0 / sizeof(dummydata) );
	vpx_reader reader;
	int ret = vpx_reader_init(&reader, bufferO, w.pos, 0, 0 );
	double startDec = GetAbsoluteTime();
	bitno = 0;
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		int bits = 8;
		int bit;
		uint8_t data = 0;
#ifdef TEST_RANDOM_PROBS
		int prob = dummyprobs[bitno++] = rand()&0xff;
#else
		int prob = arbitrary_prob;
#endif

		for (bit = bits - 1; bit >= 0; bit--)// Arbitrary, for testing
			data = (data<<1) | vpx_read(&reader, prob);
		if( data != dummydata[i] )
		{
			fprintf( stderr, "Disagree at %d (%08x != %08x)\n", i, data, dummydata[i] );
			return -9;
		}
	}
	double endDec = GetAbsoluteTime();

	printf( "Matching %d bytes\n", i );
	printf( "Encode Time:  %.3fms (%.3f MBytes/s)\n", (endEnc - startEnc)*1000.0, (sizeof(dummydata)/1024/1024)/(endEnc - startEnc) );
	printf( "Decode Time: %.3fms (%.3f MBytes/s)\n", (endDec - startDec)*1000.0, (sizeof(dummydata)/1024/1024)/(endDec - startDec) );

	return 0;
}
