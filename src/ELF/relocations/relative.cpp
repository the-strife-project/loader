#include "../ELF.hpp"

void ELF::relocateRelative(RELA* entry) {
	// Offset ok?
	if(!pages.has(PAGE(entry->r_offset))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	uint64_t local = pages[PAGE(entry->r_offset)];
	local |= PAGEOFF(entry->r_offset);

	// Resolve relative address
	if(!pages.has(PAGE(entry->r_addend))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	uint64_t abs = pages[PAGE(entry->r_addend)];
	abs |= PAGEOFF(entry->r_addend);

	// Perform the relocation
	*(uint64_t*)local = abs;
}
