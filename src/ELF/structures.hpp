#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#define MAGIC 0x464C457F

struct ELFHeader {
	char e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} __attribute__ ((packed));

#define IDENT_64 2
#define IDENT_LITTLE_ENDIAN 1
#define IDENT_X86_64 0x3E

struct PHDR {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr, undefined;
	uint64_t p_filesz, p_memsz;
	uint64_t p_align;

	struct Type {
		enum {
			_NULL,
			LOAD,
			DYNAMIC,
			INTERP,
			NOTE,
			RELRO = 0x6474E552
		};
	};
} __attribute__ ((packed));

struct SHDR {
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;

	struct Type {
		// Pretty much only those I care about
		enum {
			STRTAB = 0x3,
			DYNSYM = 0xB
		};
	};
} __attribute__ ((packed));

struct Symbol {
	uint32_t st_name;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndx;
	uint64_t st_value;
	uint64_t st_size;
} __attribute__ ((packed));

struct Dynamic {
	uint64_t d_tag;
	union {
		uint64_t d_value;
		uint64_t d_addr;
	} d_un;

	struct Tag {
		enum {
			DT_NEEDED = 0x1
		};
	};
} __attribute__ ((packed));

struct RelocationEntry {
	uint64_t r_offset;
	uint64_t r_info;
	uint64_t r_addend;
} __attribute__ ((packed));

#endif
