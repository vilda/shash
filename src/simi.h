/*
 * Implementation of the M. Charikar's document similarity hash.
 *
 * FAST 64bit version with no word stemming, stop words, token weights.
 *
 * Public domain
 * 2007 Viliam Holub
 */

#ifndef _SIMI_H_
#define _SIMI_H_

#include <stdint.h>

int hamming_dist( uint64_t a1, uint64_t a2);
uint64_t charikar_hash64( const char *data, long data_length);

#endif /* _SIMI_H_ */
