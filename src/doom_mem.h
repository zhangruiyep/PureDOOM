
#ifndef __DOOM_MEM_H__
#define __DOOM_MEM_H__

#include "mem_section.h"

void * doom_mem_malloc(size_t size);
void doom_mem_free(void * ptr);
void * doom_mem_calloc(uint32_t count, uint32_t size);

#endif
