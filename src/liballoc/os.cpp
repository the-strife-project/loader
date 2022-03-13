#include "liballoc_1_1.h"
#include <PageAlloc/PageAlloc.hpp>

// OS-dependent things for liballoc

int liballoc_lock() { return 0; }
int liballoc_unlock() { return 0; }

void* liballoc_alloc(size_t npages) { return allocp(npages); }

int liballoc_free(void* ptr, size_t npages) {
	freep(ptr, npages);
	return 0;
}

// C++
// l* (for loader 🤪) memory functions
void* operator new(size_t sz) { return lmalloc(sz); }
void* operator new[](size_t sz) { return lmalloc(sz); }
void operator delete(void* p) { lfree(p); }
void operator delete[](void* p) { lfree(p); }
