#include "ELF.hpp"
#include <basics.hpp>

// This doesn't actually deserve its own file but bleh
void ELF::setPermissions() {
	for(size_t i=0; i<nphdrs; ++i) {
		PHDR* phdr = &phdrs[i];
		// Safe since parse.cpp

		uint64_t page = PAGE(phdr->p_vaddr);
		size_t npages = phdr->npages();
		for(size_t i=0; i<npages; ++i) {
			perms[page] = phdr->p_flags & 0b111;
			page += PAGE_SIZE;
		}
	}
}
