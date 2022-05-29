#include "ELF.hpp"

void ELF::give(ELF& elf, uint64_t base) {
	// Look for them symbols
	for(auto const& x : elf.getDynamicSymbols())
		whoGives[x.f] = base + x.s;
}

// Two supported relocation types
// Don't know the difference at all
// First one seems to be in .rela.plt, second one in .rela.dyn
#define R_X86_64_JUMP_SLOT 0x7
#define R_X86_64_GLOB_DAT 0x6
#define R_X86_64_RELATIVE 0x8
#define R_X86_64_64 0x1

void ELF::relocateRELA(RELA* entries, size_t nentries) {
	for(size_t i=0; i<nentries; ++i) {
		RELA* entry = &entries[i];
		uint32_t type = entry->r_info & 0xFFFFFFFF;

		switch(type) {
		case R_X86_64_RELATIVE:
			this->relocateRelative(entry);
			break;
		case R_X86_64_JUMP_SLOT: // fallthrough
		case R_X86_64_64: // fallthrough
		case R_X86_64_GLOB_DAT:
			this->relocateSymbol(entry);
			break;
		default:
			error = std::Loader::Error::UNSUPPORTED_RELOCATION;
			break;
		}
	}
}

void ELF::relocation() {
	SHDR* plt = mapSections[".rela.plt"];
	if(plt) {
		RELA* entries = (RELA*)(data + plt->sh_offset);
		size_t nentries = plt->sh_size / sizeof(RELA);
		relocateRELA(entries, nentries);
	}

	SHDR* dyn = mapSections[".rela.dyn"];
	if(dyn) {
		RELA* entries = (RELA*)(data + dyn->sh_offset);
		size_t nentries = dyn->sh_size / sizeof(RELA);
		// Hacking: skip first entry. Why? idk
		// It has a weird relocation type (R_X86_64_RELATIVE)
		++entries;
		--nentries;
		if(nentries)
			relocateRELA(entries, nentries);
	}
}
