/*
 * Implementations of the M. Charikar's [1] document similarity hash.
 *
 * Public domain
 * 2007 Viliam Holub
 *
 * Change:
 * 	- meaningful_char function to filter input characters
 * 	- is_stopword function to apply the stopword filtering
 * 	- word_stemming function to apply the word stemming technique
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
 * token_start	first token's character
 * token_length token lenght
 */
static bool
get_next_token( const char **string,
		const char *string_end,
		const char **token_start,
		size_t *token_length)
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


/** Word stemming: Groups words with the same conceptual meaning together.
 * Usually this is implemented by removing the word suffix.
 * Depends heavily on a language, therefore this function is a stub.
 * To change the word, simply modify the passed parameters.
 */
static void
word_stemming( const char **token_start, size_t *token_length)
{
	/* Nothing */
}


/** Stopword test.
 * The stopword is a frequent word with no useful information.
 * Stopwords differs across languages: This function is a stub.
 * Token is passed in a form of 64-bit hash.
 */
static bool
is_stopword( uint64_t token)
{
	return false;
}


/** Token weight.
 */
static int
token_weight( uint64_t token)
{
	return 1;
}


/** Calculates the similarity hash with super-tokens.
 */
uint64_t
charikar_hash64_wide( const char *data, long data_length, int stoken_size)
{
	const char *data_end = data +data_length;

	/* Clear histogram */
	int hist[ 64];
	memset( hist, 0, sizeof( hist));

	/* Token position and length */
	const char *token_start;
	size_t token_length;

	/* Token buffer to create super-tokens */
	uint32_t token_buf[ 2*stoken_size];
	memset( token_buf, 0, sizeof( token_buf));

	/* Over all tokens... */
	while ((get_next_token( &data, data_end, &token_start, &token_length))) {
		/* Word stemming */
		word_stemming( &token_start, &token_length);

		/* Calculate token hash */
		uint64_t token = get_hashed_token( token_start, token_length);
		dprintf( " token: %016llx %s\n", (long long)token, string( token_start, token_length));

		/* Check for stopwords */
		if (is_stopword( token)) {
			dprintf( "stopword\n");
			continue;
		}

		/* Get token weight */
		int weight = token_weight( token);

		/* Rotate the buffer of tokens */
		if (stoken_size != 1)
			for (int c=0; c<stoken_size-1; c++) {
				token_buf[ c*2  ] = token_buf[ c*2+2];
				token_buf[ c*2+1] = token_buf[ c*2+3];
			}

		/* Write the new token at the end of the buffer */
		token_buf[ (stoken_size-1)*2  ] = token>>32;
		token_buf[ (stoken_size-1)*2+1] = token&0xffffffff;

		/* Calculate a hash of the super-token */
		uint32_t h1=0x2c759c01; /* Dummy init values */
		uint32_t h2=0xfef136d7;
		hashword2( token_buf, stoken_size*2, &h1, &h2);
		/* Concatenate results to create a super-token */
		uint64_t stoken = ((uint64_t)h1 << 32) +h2;

		dprintf( "stoken: %016llx\n", stoken);

		/* Update histogram */
		for (int c=0; c<64; c++)
			hist[ c] += (stoken & ((uint64_t)1 << c)) == 0 ? -weight : weight;
	}

	/* Calculate a bit vector from the histogram */
	uint64_t simhash=0;
	for (int c=0; c<64; c++)
		simhash |= (uint64_t)(hist[ c]>=0) << c;
	
	return simhash;
}
