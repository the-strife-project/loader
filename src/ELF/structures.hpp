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
} __attribute__((packed));

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
} __attribute__((packed));

#endif
