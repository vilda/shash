/*
 * Implementations of M. Charikar's document similarity hash.
 *
 * Public domain
 * 2007 Viliam Holub
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

#include "simi.h"
#include "simiw.h"

/** Prints a message to stderr and exits.
 */
void
die( int ex, const char *fmt, ...)

{
	va_list ap;
	
	fflush( stdout);

	if (fmt && *fmt)
	{
	        va_start( ap, fmt);
		vfprintf( stderr, fmt, ap);
		va_end( ap);
		
		fprintf( stderr, "\n");
	}
	
	exit( ex);
}

/** Reads the file specified.
 */
void
get_file( char *file_name, char **data, long *file_length) {
	/* Open input file */
	FILE *input = fopen( file_name, "r");
	if (!input)
		die( 1, "Can't open file \"%s\": %m.", file_name);

	/* Get file length */
	fseek( input, 0, SEEK_END);
	*file_length = ftell( input);
	fseek( input, 0, SEEK_SET);

	/* Get the buffer */
	*data = (char *)malloc( *file_length);
	if (!*data)
		die( 2, "Out of memoru.");

	/* Read the file */
	size_t readed = fread( *data, 1, *file_length, input);
	if (readed != *file_length)
		/* EOF cannot happen */
		die( 3, "Error reading file \"%s\": %m.", file_name);

	/* Close the file */
	if (fclose( input) == EOF)
		die( 4, "Can't close file \"%s\": %m.", file_name);
}


void
print_help( void)
{
	fprintf( stderr,
			"Usage: simi [-d] [-s size] files...\n"
			"Print Charikar's document similarity hash and compute Hamming distance\n\n"
			"\t-d\tcalculate the hamming distance of two document hashes\n"
			"\t-q\tbe quiet when computing difference\n"
			"\t-s num\tspecify the size of super-tokens\n"
			"\t-h\tthis help\n");
	exit( 5);
}


int
main( int argc, char **argv)
{
	/* Print help if there are no parameters */
	if (argc < 2)
		print_help();

	/* Parse input line */
	int opt;
	int stoken_size = 1;
	bool diff = false;
	bool quiet = false;
	while (opt = getopt( argc, argv, "dqs:"), opt != -1)
		switch (opt) {
			case 's':
				stoken_size = atoi( optarg);
				break;
			case 'd':
				diff = true;
				break;
			case 'q':
				quiet = true;
				break;
			case 'h':
			default:
				print_help();
				/* Unreachable */
		}

	if (stoken_size <= 0) {
		fprintf( stderr, "Error: Invalid super-token size\n");
		return 6;
	}

	if (diff) {
		if (argc-optind < 2) {
			fprintf( stderr, "Error: At least two files must be specified.");
			return 7;
		}
		argc = optind+2;
	}

	/* Do the work */
	uint64_t hash1, hash2;
	for (int c=optind; c<argc; c++) {
		char *data;
		long data_length;
		get_file( argv[ c], &data, &data_length);
		hash2 = hash1;
		hash1 = stoken_size == 1
			? charikar_hash64( data, data_length) /* Fast version */
			: charikar_hash64_wide( data, data_length, stoken_size); /* Fast version */
		free( data);
		if (!quiet)
			printf( "%016llx %s\n", (long long)hash1, argv[ c]);
	}

	/* Print results */
	if (diff) {
		uint64_t hashx = hash1^hash2;
		int hamm = hamming_dist( hash1, hash2);

		if (quiet) {
			printf( "%d\n", hamm);
		} else {
			printf( "%016llx\n", (long long)hashx);
			printf( "difference: %d\n", hamm);
		}
	}

	return 0;
}
