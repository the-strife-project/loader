#include "../ELF.hpp"

void ELF::relocateRelative(RELA* entry) {
	// Offset ok?
	uint64_t local = pages[entry->r_offset & ~0xFFF];
	if(!local) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	local |= entry->r_offset & 0xFFF;

	// Resolve relative address
	uint64_t abs = pages[entry->r_addend & ~0xFFF];
	abs |= entry->r_addend & 0xFFF;

	// Perform the relocation
	*(uint64_t*)local = abs;
}
