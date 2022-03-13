#ifndef ELF_HPP
#define ELF_HPP

#include <types>
#include <map>
#include "structures.hpp"

class ELF {
public:
	// virt (process) -> virt (loader)
	typedef std::unordered_map<uint64_t, uint64_t> pages_t;

private:
	char* data;
	size_t sz;

	bool isELF, is64, isLE, isArch;
	bool invalidOffset;
	uint64_t entry = ~0;

	bool noPHDRs;
	PHDR* phdrs;
	size_t nphdrs;

	pages_t pages;

	inline bool validOffset(size_t off) { return off < sz; }

public:
	ELF(void* data, size_t sz)
		: data((char*)data), sz(sz)
	{}

	void parseAndLoad();
	inline pages_t& getPages() { return pages; }
	inline uint64_t getEntry() const { return entry; }

	size_t howItWent() const;
};

#endif
