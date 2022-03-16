#include "ELF.hpp"
#include <basics.hpp>

void ELF::parseSections() {
	ELFHeader* header = (ELFHeader*)data;
	nsections = header->e_shnum;
	if(!nsections) {
		error = std::Loader::Error::NO_SECTIONS;
		return;
	}

	// Check if last section is within bounds
	if(!validOffset(header->e_shoff + (nsections*sizeof(SHDR) - 1))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	sections = (SHDR*)(data + header->e_shoff);
	for(size_t i=0; i<nsections; ++i) {
		SHDR* section = &sections[i];
		if(section->sh_size == 0) continue;

		if(!validOffset(section->sh_offset + section->sh_size - 1)) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		// Find .shstrtab
		if(section->sh_name == 1)
			shstrtab = section;
	}

	if(!shstrtab) {
		error = std::Loader::Error::NO_SHSTRTAB;
		return;
	}

	if(!safeStringsSection(shstrtab)) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	// Again, looking for names
	for(size_t i=0; i<nsections; ++i) {
		SHDR* section = &sections[i];

		// Name ok?
		if(section->sh_name >= shstrtab->sh_size) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		char* name = data + shstrtab->sh_offset + section->sh_name;
		mapSections[name] = section;
	}
}
