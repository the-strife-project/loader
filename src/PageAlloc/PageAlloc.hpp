#ifndef PAGE_ALLOC_HPP
#define PAGE_ALLOC_HPP

#include <types>

/*
	No stdlib, no proper allocator
	Gotta have some way to save stuff tho
	This is a very simple bitmap allocator
*/

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

void* allocp(size_t npages=1);
void* callocp(size_t npages=1);
void freep(void*, size_t npages=1);

#endif
