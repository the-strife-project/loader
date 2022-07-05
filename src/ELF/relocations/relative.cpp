#include "../ELF.hpp"

void ELF::relocateRelative(RELA* entry) {
	// Offset ok?
	if(!pages.has(entry->r_offset & ~0xFFF)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	uint64_t local = pages[entry->r_offset & ~0xFFF];
	local |= entry->r_offset & 0xFFF;

	// Resolve relative address
	if(!pages.has(entry->r_addend & ~0xFFF)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	uint64_t abs = pages[entry->r_addend & ~0xFFF];
	abs |= entry->r_addend & 0xFFF;

	// Perform the relocation
	*(uint64_t*)local = abs;
}
