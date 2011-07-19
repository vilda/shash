/*
 * Implementations of the M. Charikar's document similarity hash.
 *
 * Public domain
 * 2007 Viliam Holub
 */

#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#include "lookup3.h"


/* Debug mode */
#define DEBUG 0

#if DEBUG
#	include <stdio.h>
#	define dprintf(...) fprintf( stderr, __VA_ARGS__)
#else
#	define dprintf(...)
#endif


/** Filters unimportant characters.
 * Now important characters are alphanumeric and those with the most
 * significant bit set (utf-8 friendly).
 */
static bool
meaningful_char( char c)
{
	return isalnum( c) || (c&0x80);
}


#if DEBUG
/** Converts a pair of address and length to a C-string.
 * Returns an internal buffer!
 */
static const char *
string( const char *string_start, size_t string_length)
{
	static char sbuf[ 1024];
	int c;
	int stop = string_length < 1023 ? string_length : 1023;
	for (c=0; c<stop; c++)
		sbuf[ c] = string_start[ c];
	sbuf[ c] = '\0';

	return sbuf;
}
#endif

/** Returns next token.
 * string	current data pointer
 * string_end	pointer one character behind the end of data
 * token_start	output hash value of the token
 * token_length
 */
static bool
get_next_token( const char **string, const char *string_end, const char **token_start, size_t *token_length)
{
	const char *start = *string;

	/* Skip white space */
	while (start != string_end && !meaningful_char( *start))
		start++;

	/* At the end of the string? */
	if (start == string_end) {
		*string = start;
		dprintf( " token: end\n");
		return false;
	}
	
	/* Read the string */
	const char *s = start+1;
	while (s != string_end && meaningful_char( *s)) {
		s++;
	}

	*string = s;
	*token_start = start;
	*token_length = s-start;
	return true;
}


/** Hashes the given token.
 */
static uint64_t
get_hashed_token( const char *token_start, size_t token_length)
{
	uint32_t h1 = 0xac867c1d; /* Dummy init values */
	uint32_t h2 = 0x5434e4c4;
	hashlittle2( token_start, token_length, &h1, &h2);
	return ((uint64_t)h1 << 32) +h2;
}


/** Updates histogram with weight==1.
 * Tricky implementation, optimized for speed.
 */
static inline void
update_hist( int *hist, uint64_t token)
{
	uint32_t t = token;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1;
	t = token>>32;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist++ += t & 1; t >>= 1;
	*hist   += t & 1;
}


/** Calculates the hamming distance.
 */
int
hamming_dist( uint64_t a1, uint64_t a2)
{
	uint32_t v1 = a1^a2;
	uint32_t v2 = (a1^a2)>>32;

	v1 = v1 - ((v1>>1) & 0x55555555);
	v2 = v2 - ((v2>>1) & 0x55555555);
	v1 = (v1 & 0x33333333) + ((v1>>2) & 0x33333333);
	v2 = (v2 & 0x33333333) + ((v2>>2) & 0x33333333);
	int c1 = ((v1 + (v1>>4) & 0xF0F0F0F) * 0x1010101) >> 24;
	int c2 = ((v2 + (v2>>4) & 0xF0F0F0F) * 0x1010101) >> 24;

	return c1+c2;
}


/** Calculates the similarity hash.
 */
uint64_t
charikar_hash64( const char *data, long data_length)
{
	const char *data_end = data +data_length;

	/* Clear histogram */
	int hist[ 64];
	memset( hist, 0, sizeof( hist));

	/* Token position and length */
	const char *token_start;
	size_t token_length;

	/* Over all tokens... */
	int tokens = 0;
	while ((get_next_token( &data, data_end, &token_start, &token_length))) {
		/* Calculate token hash */
		uint64_t token = get_hashed_token( token_start, token_length);
		dprintf( " token: %016llx %s\n", (long long)token, string( token_start, token_length));

		/* Update histogram, weigth==1 */
		update_hist( hist, token);
		tokens++;
	}
	tokens /= 2;

	/* Calculate a bit vector from the histogram */
	uint64_t simhash=0;
	for (int c=0; c<64; c++)
		simhash |= (uint64_t)((hist[ c]-tokens)>=0) << c;

	return simhash;
}
