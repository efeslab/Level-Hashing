#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*  Cache line flush: 
    clflush can be replaced with clflushopt or clwb, if the CPU supports clflushopt or clwb.  
*/
#define asm_clflush(addr)                   \
({                              \
    __asm__ __volatile__ ("clflush %0" : : "m"(*addr)); \
})

/*  Memory fence:  
    mfence can be replaced with sfence, if the CPU supports sfence.
*/
#define asm_mfence()                \
({                      \
    __asm__ __volatile__ ("mfence":::"memory");    \
})


void pflush(uint64_t *addr);

void init_pmalloc(const char *fname);

void *pmalloc_lvl(size_t nbytes);

void pfree(void *ptr, size_t nelem);
