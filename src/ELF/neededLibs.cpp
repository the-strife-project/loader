#include "ELF.hpp"

void ELF::findNeededLibs() {
	// .dynamic section?
	SHDR* dynamic = mapSections[".dynamic"];
	// Safe to dereference, checked in sections.cpp

	if(!dynamic)
		return;

	size_t ndynamic = dynamic->sh_size / sizeof(Dynamic);
	Dynamic* dynamics = (Dynamic*)(data + dynamic->sh_offset);
	// Safe as well.

	if(!ndynamic)
		return;

	// This might have already been checked in findExports. Don't care.
	dynstr = mapSections[".dynstr"];
	if(!dynstr) {
		// There are dynamics, but no names. Biggie
		error = std::Loader::Error::NO_DYNSTR;
		return;
	}

	if(!safeStringsSection(dynstr)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	for(size_t i=0; i<ndynamic; ++i) {
		if(dynamics[i].d_tag != Dynamic::Tag::DT_NEEDED)
			continue;

		size_t val = dynamics[i].d_un.d_value;
		// Name ok?
		if(val >= dynstr->sh_size) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		char* name = data + dynstr->sh_offset + val;
		libs.insert(name);
	}
}
