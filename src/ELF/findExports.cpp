#include "ELF.hpp"

void ELF::findExports() {
	// Exported symbols are at .dynsym section
	SHDR* dynsymh = mapSections[".dynsym"];
	if(!dynsymh) {
		// No exports, no biggie
		return;
	}

	size_t ndynsyms = dynsymh->sh_size / sizeof(Symbol);
	//dynsym is perfectly safe to dereference, since size was checked in sections.cpp

	// Names?
	dynstr = mapSections[".dynstr"];
	if(!dynstr) {
		// There are exports, but no names. Biggie
		error = std::Loader::Error::NO_DYNSTR;
		return;
	}

	if(!safeStringsSection(dynstr)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	// Alright. Everything is safe. Paranoia is almost over.

	Symbol* syms = (Symbol*)(data + dynsymh->sh_offset);
	for(size_t i=0; i<ndynsyms; ++i) {
		Symbol* sym = &syms[i];

		// Name ok?
		if(sym->st_name >= dynstr->sh_size) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		char* name = data + dynstr->sh_offset + sym->st_name;

		// sym->st_value contains function/variable offset with respect to "something"
		// The standard is confusing, I'm not sure what it's relative to
		// Here, I'm assuming it's relative to base on memory. If it turns out
		//   it's not, program will crash, but loader will be safe either way
		dynsyms[name] = sym->st_value;
	}
}
