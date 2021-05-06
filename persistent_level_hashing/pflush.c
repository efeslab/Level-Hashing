#include <assert.h>

#include "pflush.h"
#include "libpmem.h"
#include "libpmemobj.h"
/* Note that we refered to the implementation code of pflush function in Quartz
*/

/*
Function: pflush() 
        Flush a cache line with the address addr;
*/
void pflush(uint64_t *addr)
{
    // iangneal: just use PMDK.
    // Should flush just one line, regardless of offset.
    pmem_flush(addr, 1);
}

static PMEMobjpool *pop;

#ifndef TYPE_OFFSET
#define TYPE_OFFSET 1012
#endif

TOID_DECLARE(struct alloc_node, TYPE_OFFSET + 0);
TOID_DECLARE(struct driver_root, TYPE_OFFSET + 1);
TOID_DECLARE(char, TYPE_OFFSET + 2);

typedef struct alloc_node {
    TOID(struct alloc_node) next;
    TOID(char) data;
} alloc_node_t;

typedef struct driver_root {
    TOID(struct alloc_node) head;
    TOID(struct alloc_node) tail;
} driver_root_t;

POBJ_LAYOUT_BEGIN(driver);
POBJ_LAYOUT_ROOT(driver, driver_root_t);
POBJ_LAYOUT_END(driver);

void init_pmalloc(const char *fname)
{
    pop = pmemobj_create(fname, POBJ_LAYOUT_NAME(driver), PMEMOBJ_MIN_POOL, 0666);
    if (pop == NULL) {
	    pop = pmemobj_open(fname, POBJ_LAYOUT_NAME(driver));
    }

    assert(pop && "Cannot be null!");
}

void *pmalloc_lvl(size_t nbytes) {
    TOID(struct driver_root) root = POBJ_ROOT(pop, struct driver_root);

    TX_BEGIN(pop) {
        TX_ADD(root);

        TOID(struct alloc_node) new_node = TX_ZNEW(struct alloc_node);

        D_RW(new_node)->data = TX_ALLOC(char, nbytes);

        if (!TOID_IS_NULL(D_RO(root)->tail)) {
            // Set tail next
            TOID(struct alloc_node) old_tail = D_RO(root)->tail;
            assert(TOID_IS_NULL(D_RO(old_tail)->next));

            TX_ADD(old_tail);

            D_RW(old_tail)->next = new_node;

        }

        D_RW(root)->tail = new_node;

        if (TOID_IS_NULL(D_RO(root)->head)) {
            D_RW(root)->head = new_node;
        }
    } TX_END

    void *alloc_addr = (void*)pmemobj_direct(D_RO(D_RO(root)->tail)->data.oid);
    printf("\talloc-ed %p, len %lx\n", alloc_addr, nbytes);

    return alloc_addr;
}

void pfree(void *ptr, size_t nelem) {
    // Do nothing!
    fprintf(stderr, "\t\t%s remains unimplemented!\n", __FUNCTION__);
}
