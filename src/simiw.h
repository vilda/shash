/*
 * Implementations of the M. Charikar's document similarity hash.
 *
 * Public domain
 * 2007 Viliam Holub
 */

#ifndef _SIMIW_H_
#define _SIMIW_H_

#include <stdint.h>

uint64_t charikar_hash64_wide( const char *data, long data_length, int stoken_size);

#endif /* _SIMIW_H_ */
