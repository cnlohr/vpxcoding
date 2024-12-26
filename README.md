# vpxcoding single-file-header C library

Single file header form of the arithmetic coder from [libvpx](https://github.com/webmproject/libvpx) as a general purpose compression/decompression of bitstreams algorithm.

The idea of this coding is, given:

1. A bitstream
2. Knowledge about how likely the next bit is to be a 0 or 1 (written as a probability from 0..255)

You can optimally code an output bitstream, with compression better than huffman trees by using arithmetic coding.  Please note this is **not** a replacement for something like lz77, zstd, zlib, etc.  But **is** a replacement for huffman coding.  This ONLY covers optimal symbol expression.  If there is data-similarity that must be compressed by another algorithm.  In general, you will want to get rid of whatever entropy you can before applying this compression technique. I.e. you can't just use this to compress text.

It's also reaonsably fast. Not great, but not bad.

```
Input  Len: 16777216 bytes
Output Len: 14104056 bytes
Relative Size: 84.07 %
Matching 16777216 bytes
Encode Time:  375.116ms (42.653 MBytes/s)
Decode Time: 537.677ms (29.758 MBytes/s)
```
(on a AMD Ryzen 7 5800X, GCC 11.4.0, -O2)

## Example

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
	// Higher numbers mean 0 is more likely, 0.5 = equally possible 1 and 0.
	float probability01 = (chance of next bit being 0);
	int probability = probability01 * 257 - 0.5;

	// Must bound.  Probability is only a uint8_t
	if( probability > 255 ) probability = 255;
	if( probability < 0 ) probability = 0;
```

NOTE: This is found emperically.  It may not be correct or as-designed.


## Overall Properties

![Overall](https://github.com/user-attachments/assets/55d98d1d-9fc9-4bb2-a436-16dd0fbc603d)

![Edges](https://github.com/user-attachments/assets/c18f296a-d2af-4d7d-84a3-ef145f01a66c)

![Optimal](https://github.com/user-attachments/assets/d2315457-68a6-460e-aaa2-73ba25c0b0aa)


## Special Thanks


Whoever the original author actually was.  At the very least thank you for making sure it's properly licensed.

[@danielrh](https://github.com/danielrh) for alerting me to this, and helping me understand its application outside of libvpx.
 * https://github.com/danielrh/arithmetic_coding_tutorial 
 * https://github.com/danielrh/losslessh264

 * This video is a GREAT introduction to Huffman, Arithmetic Coding, And ANS [Better than Huffman](https://www.youtube.com/watch?v=RFWJM8JMXBs)

These two youtube videos were really good, but don't explain this specific implementation.
 * [(IC 5.1) Arithmetic coding - introduction](https://www.youtube.com/watch?v=ouYV3rBtrTI)
 * [(IC 5.2) Arithmetic coding - Example #1](https://www.youtube.com/watch?v=7vfqhoJVwuc)
 * [(IC 5.3) Arithmetic coding - Example #2](https://www.youtube.com/watch?v=CXCWQy9N2ag)
 * [(IC 5.4) Why the interval needs to be completely contained](https://www.youtube.com/watch?v=jHS8-rmEo5k)
 * [(IC 5.5) Rescaling operations for arithmetic coding](https://www.youtube.com/watch?v=t8_198HHSfI)
 * This continues on for 13? episodes?


