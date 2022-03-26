#include "ELF.hpp"

void ELF::give(ELF& elf, uint64_t base) {
	// Look for them symbols
	for(auto const& x : elf.getDynamicSymbols())
		whoGives[x.f] = base + x.s;
}

#define R_X86_64_JUMP_SLOT 0x7

void ELF::relocation() {
	// Find GOT entries
	SHDR* got = mapSections[".rela.plt"];

	if(!got)
		return;
	// Might need to check REL as well, ".rel.dyn"

	// Safe to dereference :)

	// At this point, there has to be something in "libs"
	if(!libs.size())
		return;

	RelocationEntry* entries = (RelocationEntry*)(data + got->sh_offset);
	size_t nentries = got->sh_size / sizeof(RelocationEntry);

	for(size_t i=0; i<nentries; ++i) {
		RelocationEntry* entry = &entries[i];

		uint32_t type = entry->r_info & 0xFFFFFFFF;
		if(type != R_X86_64_JUMP_SLOT) {
			error = std::Loader::Error::UNSUPPORTED_RELOCATION;
			return;
		}

		uint32_t symidx = entry->r_info >> 32;

		// Name ok?
		// dynstr is safe since neededLibs() finished without errors
		// neededLibs() has been executed because "libs" is not empty
		if(symidx >= ndynsyms) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		// Safe to dereference since it was checked in findExports
		std::string name = data + dynstr->sh_offset + rawdynsyms[symidx].st_name;

		// Is the symbol exported by a needed library?
		if(!whoGives[name]) {
			error = std::Loader::Error::FAILED_RELOCATION;
			return;
		}

		// Offset ok?
		uint64_t local = pages[entry->r_offset & ~0xFFF];
		if(!local) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		// Alright, perform the relocation
		local |= entry->r_offset & 0xFFF;
		*(uint64_t*)local = whoGives[name];
	}
}
