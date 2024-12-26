# vpxcoding_sfh

Single file header form of the arithmetic coder from [libvpx](https://github.com/webmproject/libvpx) as a general purpose compression/decompression algorithm.

The idea of this coding is, given:

1. A bitstream
2. Knowledge about how likely the next bit is to be a 0 or 1

You can optimally code an output bitstream

## Overall Properties

![Overall](https://private-user-images.githubusercontent.com/2748168/398312513-4c8addfd-bfac-40de-a928-53b6a75f4889.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUwMTMwNjAsIm5iZiI6MTczNTAxMjc2MCwicGF0aCI6Ii8yNzQ4MTY4LzM5ODMxMjUxMy00YzhhZGRmZC1iZmFjLTQwZGUtYTkyOC01M2I2YTc1ZjQ4ODkucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNCUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjRUMDM1OTIwWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9M2UxMzc2MDQ5MWRkMjlhMzkzZTEzNWM5Y2U1ODdhNzE1MDdmOWJlOTc4MjBmZDczOWNlMmUzZTJhMzQwMTk0NCZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.6t_bRDe_MDCwKUozlnSwKfeOw9jNrCHzLCan_Jd-9EU)

![Edges](https://private-user-images.githubusercontent.com/2748168/398312509-78aca02b-c0ea-4bd9-b8b7-cfa7d4e744bf.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUwMTMwNjAsIm5iZiI6MTczNTAxMjc2MCwicGF0aCI6Ii8yNzQ4MTY4LzM5ODMxMjUwOS03OGFjYTAyYi1jMGVhLTRiZDktYjhiNy1jZmE3ZDRlNzQ0YmYucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNCUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjRUMDM1OTIwWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9ZDI5NDQ4NmRjNzEyY2U3YTUzNWQyOWE0ZDFjMjU2MzZhMTA5ZmYwNjk0OTg3NWZiOGY5YzJkNWZiMTdjMWYzYyZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.dqcjEJi90PZRGPey3dNGE7eL5XFbEMUdj8vbOIdd7zY)

![Optimal](https://private-user-images.githubusercontent.com/2748168/398316165-423ae5b1-fde5-41d7-b938-f526a421d383.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MzUwMTQyMjEsIm5iZiI6MTczNTAxMzkyMSwicGF0aCI6Ii8yNzQ4MTY4LzM5ODMxNjE2NS00MjNhZTViMS1mZGU1LTQxZDctYjkzOC1mNTI2YTQyMWQzODMucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MTIyNCUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDEyMjRUMDQxODQxWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9ZTdmYzk5NzZhMzM2YjRkN2UxOGVhODE4OGJlZDIzZDllMDcxYmZhNzEzNTFlODIyNTZiOWY0ZGZhYTFkYzBlMiZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.jl6elWvXzKHABeh1pVHkZgsIUgE2ETwW23RoB9xJj_A)

It appears the optimal bit selection is:

```c
	probability = (chance of next bit being 0) * 257 - 0.5;
	if( probability > 255 ) probability = 255;
	if( probability < 0 ) probability = 0;
```

(PLEASE NOTE: This may be incorrect, or non-optimal. This was just emperically found.)

## Special Thanks

[@danielrh](https://github.com/danielrh)

## Resources:
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


