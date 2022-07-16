#include "ELF.hpp"
#include <basics.hpp>
#include <map>
#include <algorithm>
#include <syscalls>

void ELF::parseAndLoad() {
	// Load header
	if(!validOffset(sizeof(ELFHeader))) {
		error = std::Loader::Error::NOT_ELF;
		return;
	}

	ELFHeader* header = (ELFHeader*)data;
	// Magic number, strict aliasing violation but who cares
	if(*(uint32_t*)header->e_ident != MAGIC) {
		error = std::Loader::Error::NOT_ELF;
		return;
	}
	if(header->e_ident[4] != IDENT_64) {
		error = std::Loader::Error::NOT_64;
		return;
	}
	if(header->e_ident[5] != IDENT_LITTLE_ENDIAN) {
		error = std::Loader::Error::NOT_LE;
		return;
	}
	if(header->e_machine != IDENT_X86_64) {
		error = std::Loader::Error::BAD_ARCH;
		return;
	}

	// Entry point
	entry = header->e_entry;

	// Program headers
	nphdrs = header->e_phnum;
	if(!nphdrs) {
		error = std::Loader::Error::NO_PHDRS;
		return;
	}

	// Safe?
	if(!safeProduct(nphdrs, sizeof(PHDR))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	if(!validRegion(header->e_phoff, nphdrs*sizeof(PHDR))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	phdrs = (PHDR*)(data + header->e_phoff);
	for(size_t i=0; i<nphdrs; ++i) {
		PHDR* phdr = &phdrs[i];
		// Check for valid contents
		if(!validRegion(phdr->p_offset, phdr->p_filesz)) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		// Only continue if PHDR is PT_LOAD
		if(phdr->p_type != PHDR::Type::LOAD)
			continue;

		size_t npages = phdr->npages();

		// Current destination address
		size_t vaddr = phdr->p_vaddr;
		// Current source address
		char* faddr = data + phdr->p_offset;
		// How many bytes are left to copy
		size_t remainingFile = phdr->p_filesz;
		size_t remainingMem  = phdr->p_memsz;

		while(npages--) {
			// If the page is not allocated, do it now
			size_t page = PAGE(vaddr);
			if(!pages.has(page)) {
				pages[page] = (size_t)std::mmap();
				if(!pages.has(page)) {
					// TODO: just panic and free everything
					error = std::Loader::Error::NO_MEMORY;
					HALT_AND_CATCH_FIRE();
					while(true);
				}
			}

			// Copy time
			void* dst = (void*)(pages[page] + PAGEOFF(vaddr));
			// Remaining bytes until the end of the page
			size_t szFile = std::min(PAGE_SIZE - PAGEOFF(vaddr), remainingFile);
			memcpy(dst, faddr, szFile);
			faddr += szFile;
			remainingFile -= szFile;

			size_t szMem = std::min(PAGE_SIZE - PAGEOFF(vaddr), remainingMem);
			vaddr += szMem;
			remainingMem -= szMem;
		}
	}
}
