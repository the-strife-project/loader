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

void ELF::relocateRELA(RELA* entries, size_t nentries) {
	for(size_t i=0; i<nentries; ++i) {
		RELA* entry = &entries[i];

		uint32_t type = entry->r_info & 0xFFFFFFFF;
		if(type != R_X86_64_JUMP_SLOT && type != R_X86_64_GLOB_DAT) {
			error = std::Loader::Error::UNSUPPORTED_RELOCATION;
			return;
		}

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
