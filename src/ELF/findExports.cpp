#include "ELF.hpp"

void ELF::findExports() {
	// Exported symbols are some of the entries at .dynsym section
	dynsymh = mapSections[".dynsym"];
	if(!dynsymh) {
		// No dynamic symbols, no biggie
		return;
	}

	ndynsyms = dynsymh->sh_size / sizeof(Symbol);
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

	rawdynsyms = (Symbol*)(data + dynsymh->sh_offset);
	for(size_t i=0; i<ndynsyms; ++i) {
		Symbol* sym = &rawdynsyms[i];

		// Name ok? I want them all checked
		if(sym->st_name >= dynstr->sh_size) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		// Assumption: a symbol is only exported if st_value is non-zero
		// I think if it's zero, then it's imported (relocated)
		if(!sym->st_value)
			continue;

		char* name = data + dynstr->sh_offset + sym->st_name;

		// sym->st_value contains function/variable offset with respect to "something"
		// The standard is confusing, I'm not sure what it's relative to
		// Here, I'm assuming it's relative to base on memory. If it turns out
		//   it's not, program will crash, but loader will be safe either way
		exported[name] = sym->st_value;
	}

	// TODO: not all symbols in .dynsym are exported! Some are imported! Should check st_value.
}
