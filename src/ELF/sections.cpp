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
	if(!safeProduct(nsections, sizeof(SHDR))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}
	if(!validRegion(header->e_shoff, nsections*sizeof(SHDR))) {
		error = std::Loader::Error::INVALID_OFFSET;
		return;
	}

	sections = (SHDR*)(data + header->e_shoff);
	for(size_t i=0; i<nsections; ++i) {
		SHDR* section = &sections[i];
		if(section->sh_size == 0) continue;

		// Exception: skip ".bss" section (validRegion might fail!)
		if(section->sh_type == SHT_NOBITS) continue;

		if(!validRegion(section->sh_offset, section->sh_size)) {
			error = std::Loader::Error::INVALID_OFFSET;
			return;
		}

		/*
			It's critical to find ".shstrtab". It could be done in this
			very loop. But how?
			I have absolutely no idea. I thought for a moment that sh_name=1
			for the section that has the names. Sounds convincing right? Wrong.
			I saw that in an ELF and assumed that it would be the same for all.
			It's not. It's random. LOOKS like it's the last section, but I'm
			not falling in the same pithole twice.
			Current approach: assume it's a string section, and look for its
			own name in the offset given by sh_name.
			It's the opposite of fail-proof, but the standard is of no help.
		*/
		if(!shstrtab && safeStringsSection(section)) {
			// Could be. Name ok?
			if(section->sh_name >= section->sh_size)
				continue;
			// Looks promising
			std::string name = data + section->sh_offset + section->sh_name;
			if(name == ".shstrtab") {
				// Success!
				shstrtab = section;
			}
		}
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
