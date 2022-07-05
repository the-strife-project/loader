#include "../ELF.hpp"

void ELF::relocateSymbol(RELA* entry) {
	uint32_t symidx = entry->r_info >> 32;

	if(symidx >= ndynsyms) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	// Safe to dereference since it was checked in findExports,
	//   which surely was called. Otherwise, ndynsyms would be zero, and
	//   it would've returned above.
	std::string name = data + dynstr->sh_offset + rawdynsyms[symidx].st_name;

	// Is the symbol exported by a needed library?
	if(!whoGives[name]) {
		error = std::Loader::Error::FAILED_RELOCATION;
		return;
	}

	// Offset ok?
	if(!pages.has(entry->r_offset & ~0xFFF)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	uint64_t local = pages[entry->r_offset & ~0xFFF];

	// Alright, perform the relocation
	local |= entry->r_offset & 0xFFF;
	*(uint64_t*)local = whoGives[name];
}
