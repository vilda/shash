
#ifndef _LOOKUP3_H_
#define _LOOKUP3_H_

#include <stdint.h>
#include <stdlib.h>

uint32_t hashword( const uint32_t *k, size_t length, uint32_t initval); 
void hashword2 ( const uint32_t *k, size_t length, uint32_t *pc, uint32_t *pb);   
uint32_t hashlittle( const void *key, size_t length, uint32_t initval);
void hashlittle2( const void *key, size_t length, uint32_t *pc, uint32_t *pb);
uint32_t hashbig( const void *key, size_t length, uint32_t initval);

#endif /* _LOOKUP3_H_ */
