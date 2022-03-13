#include "ELF.hpp"
#include <basics.hpp>
#include <PageAlloc/PageAlloc.hpp>
#include <map>

inline size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

void ELF::parseAndLoad() {
	// Load header
	if(!validOffset(sizeof(ELFHeader))) {
		isELF = false;
		return;
	}

	// Check magic number
	ELFHeader* header = (ELFHeader*)data;
	isELF = (*(uint32_t*)header->e_ident == MAGIC); // Magic number
	is64 = (header->e_ident[4] == IDENT_64); // 64 bits
	isLE = (header->e_ident[5] == IDENT_LITTLE_ENDIAN); // Little endian
	isArch = (header->e_machine == IDENT_X86_64); // x86_64 arch
	if(!(isELF && is64 && isLE && isArch))
		return;

	// Entry point
	entry = header->e_entry;

	// Program headers
	noPHDRs = !(nphdrs = header->e_phnum);
	if(noPHDRs) return;
	// Only have to check the last one to know all PHDRPs are safe
	if(!validOffset(header->e_phoff + (nphdrs*sizeof(PHDR) - 1))) {
		invalidOffset = true;
		return;
	}

	phdrs = (PHDR*)(data + header->e_phoff);
	for(size_t i=0; i<nphdrs; ++i) {
		PHDR* phdr = &phdrs[i];
		// Check for valid contents
		if(!validOffset(phdr->p_offset + phdr->p_filesz - 1)) {
			invalidOffset = true;
			return;
		}

		// How many pages are involved in this program header?
		size_t npages = (phdr->p_vaddr + phdr->p_memsz) & ~0xFFF;
		npages -= phdr->p_vaddr & ~0xFFF;
		npages >>= 12;

		// Current destination address
		size_t vaddr = phdr->p_vaddr;
		// Current source address
		void* faddr = data + phdr->p_offset;
		// How many bytes are left to copy
		size_t remaining = phdr->p_filesz;

		for(size_t j=0; j<npages; ++j) {
			// If the page is not allocated, do it now
			size_t page = vaddr & ~0xFFF;
			if(!pages[page])
				pages[page] = (size_t)callocp();

			// Copy time
			void* dst = (void*)(pages[page] + (vaddr & 0xFFF));
			// Remaining bytes until the end of the page
			size_t sz = min(PAGE_SIZE - (vaddr & 0xFFF), remaining);
			memcpy(dst, faddr, sz);
		}
	}
}
