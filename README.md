# vpxcoding single-file-header library.

Single file header form of the arithmetic coder from [libvpx](https://github.com/webmproject/libvpx) as a general purpose compression/decompression of bitstreams algorithm.

The idea of this coding is, given:

1. A bitstream
2. Knowledge about how likely the next bit is to be a 0 or 1 (written as a probability from 0..255)

You can optimally code an output bitstream, with compression better than huffman trees by using arithmetic coding.  Please note this is **not** a replacement for something like lz77, zstd, zlib, etc.  But **is** a replacement for huffman coding.  This ONLY covers optimal symbol expression.  If there is data-similarity that must be compressed by another algorithm.

In general, you will want to get rid of whatever entropy you can before applying this compression technique.

It's very simple, if you have a bitstream you want to encode, you can write something like:

```c
#define VPXCODING_WRITER
#include "vpxcoding.h"

int main()
{
	vpx_writer w;
	uint8_t * bufferO = malloc(2048);
	vpx_start_encode( &w, bufferO, 2048);
	vpx_write(&w, 1, 10 );
	vpx_write(&w, 0, 250);
	vpx_write(&w, 1, 128);
	vpx_stop_encode(&w);

	// w.pos contains the # of bytes written into bufferO.
}
```

Or to read, 
```c
#define VPXCODING_READER
#include "vpxcoding.h"

int main()
{
	// Assume a uint8_t* bufferI, and it's length, bufferIlen
	vpx_reader reader;
	int ret = vpx_reader_init(&reader, bufferI, bufferIlen, 0, 0 );

	int first =  vpx_read(&reader, 10);  // 1
	int second = vpx_read(&reader, 250); // 0
	int third =  vpx_read(&reader, 128); // 1
}
```


It appears the optimal bit selection is:

```c
	probability = (chance of next bit being 0) * 257 - 0.5;

	// Must bound.  Probability is only a uint8_t
	if( probability > 255 ) probability = 255;
	if( probability < 0 ) probability = 0;
```

NOTE: This is found emperically.  It may not be correct or as-designed.


## Overall Properties

![Overall](https://private-user-images.githubusercontent.com/2748168/398571327-1ef7e391-2c7e-4f97-8a4e-cff0c53cc818.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUxMjkyNDIsIm5iZiI6MTczNTEyODk0MiwicGF0aCI6Ii8yNzQ4MTY4LzM5ODU3MTMyNy0xZWY3ZTM5MS0yYzdlLTRmOTctOGE0ZS1jZmYwYzUzY2M4MTgucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjVUMTIxNTQyWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9OTFkOTgyMTY5MjJhNjk0OTNjM2VhMGI2ZDMxYzVkNTIyN2QwMmY0ZWQ1NDlmMWU1MWNhNTZmNGVkNDI3YmQxNiZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.IX-AwtKMzUBppi4N1R1tnjksyofYlQKl0izo2jKk1Eg)

![Edges](https://private-user-images.githubusercontent.com/2748168/398571325-ded30a7a-e449-4da4-865c-d450513f0139.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUxMjkyNDIsIm5iZiI6MTczNTEyODk0MiwicGF0aCI6Ii8yNzQ4MTY4LzM5ODU3MTMyNS1kZWQzMGE3YS1lNDQ5LTRkYTQtODY1Yy1kNDUwNTEzZjAxMzkucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjVUMTIxNTQyWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9YTJmM2E4YmE3NGQ1Y2EwZDFhYTk2N2Q1ZmIyMWQ5ODJmZTE1ZTZhZmQyNTM2ZTA1ODI4YjAxNWE5YzExMDgxYyZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.MEo910d7UEcy97vSx2zMIF6cBCvuRbwpt2yofZA4ROM)

![Optimal](https://private-user-images.githubusercontent.com/2748168/398571328-1c47a87f-d00f-4b7b-bf1e-1da297602786.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUxMjkyNDIsIm5iZiI6MTczNTEyODk0MiwicGF0aCI6Ii8yNzQ4MTY4LzM5ODU3MTMyOC0xYzQ3YTg3Zi1kMDBmLTRiN2ItYmYxZS0xZGEyOTc2MDI3ODYucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjVUMTIxNTQyWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9OTc3ODllOTVmZmU4MzJjZGNkNGVhOWEwMjBkNjM5MGViMGUxNjM2NDc1OTk4ZmMwYjQwNTNkZTA2OTFkNmNhMyZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.P3PlCYdhjuegYD7KD6o_LoAldsuhj8dNmVlHC33C6AQ)


## Special Thanks


Whoever the original author actually was.  At the very least thank you for making sure it's properly licensed.

[@danielrh](https://github.com/danielrh) for alerting me to this, and helping me understand its application outside of libvpx.
 * https://github.com/danielrh/arithmetic_coding_tutorial 
 * https://github.com/danielrh/losslessh264

