#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define VPXCODING_READER
#define VPXCODING_WRITER
#include "vpxcoding.h"

double GetAbsoluteTime()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return ((double)tv.tv_usec)/1000000. + (tv.tv_sec);
}

uint8_t dummydata[1024*1024*16];
int main( int argc, char ** argv )
{
	int i;
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		//dummydata[i] = rand();
		dummydata[i] = ((rand()&0xff)*(rand()&0xff)) >> 10;
	}

	vpx_writer w = { 0 };
	uint8_t * bufferO = malloc(sizeof(dummydata)*20);
	vpx_start_encode( &w, bufferO, sizeof(dummydata)*20);

	const int arbitrary_prob = 187;

	double startEnc = GetAbsoluteTime();
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		int data = dummydata[i];
		int bits = 8;
		int bit;
		for (bit = bits - 1; bit >= 0; bit--)
		{
			int outbit = (data >> bit) & 1;
			vpx_write(&w, outbit, arbitrary_prob);
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
	for( i = 0; i < sizeof(dummydata); i++ )
	{
		int bits = 8;
		int bit;
		uint8_t data = 0;
		for (bit = bits - 1; bit >= 0; bit--)// Arbitrary, for testing
			data = (data<<1) | vpx_read(&reader, arbitrary_prob);
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
