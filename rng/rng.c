#include "rng.h"
#include "../os.h"

/**
 * A simple XORshift pseudorandom number generator (PRNG) with theoretical period 2**32 - 1
 * From: Marsaglia, George, 2003: "Xorshift RNGs"
 * Retrieved from: <doi:10.18637/jss.v008.i14>
 *
 * Implemented by: Paul Karhnak (zxz2hm)
 * For: ECE 4501: Advanced Embedded Computing Systems, Spring 2024
 */
uint32_t rng(void) {
    static uint32_t CurrentRandomValue = 0;
    
    if (CurrentRandomValue == 0) {
	uint32_t CurrentTimeSeed = 0;
	while (CurrentTimeSeed == 0) {
	    CurrentTimeSeed = OS_Time(); // Ensure a nonzero seed (which an XOR shift generator requires)
	}
	CurrentRandomValue = CurrentTimeSeed;
    }

    CurrentRandomValue ^= (CurrentRandomValue << 5);
    CurrentRandomValue ^= (CurrentRandomValue >> 27);
    CurrentRandomValue ^= (CurrentRandomValue << 25);

    return CurrentRandomValue;
}

/**
 * A helper utility to get a useful direction (0, 1, 2, 3, or 4)
 * from a generated random number.
 *
 * Functionally, a wrapper for rng().
 *
 * Author: Paul Karhnak (zxz2hm)
 * For: ECE 4501: Advanced Embedded Computing Systems, Spring 2024
 */
uint32_t GetRandomDirection(void) {
    return (rng() & 255u) / 51;
}
