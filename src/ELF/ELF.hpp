#ifndef ELF_HPP
#define ELF_HPP

#include <types>
#include <set>
#include <map>
#include <string>
#include "structures.hpp"
#include <loader> // From stdlib, contains error struct

class ELF {
public:
	// virt (process) -> virt (loader)
	typedef std::unordered_map<uint64_t, uint64_t> pages_t;

	typedef std::unordered_map<std::string, SHDR*> sections_t;
	typedef std::unordered_map<std::string, uint64_t> dynsyms_t;
	typedef std::unordered_set<std::string> libs_t;

private:
	char* data = nullptr;
	size_t sz = 0;
	size_t error = std::Loader::Error::NONE;

	uint64_t entry = ~0;

	PHDR* phdrs = nullptr;
	size_t nphdrs = 0;

	pages_t pages;

	SHDR* sections = nullptr;
	size_t nsections = 0;
	SHDR* shstrtab = nullptr; // Section with names of more sections
	sections_t mapSections;

	dynsyms_t dynsyms; // Exported symbols
	SHDR* dynstr = nullptr;

	libs_t libs; // Needed libraries
	std::unordered_map<std::string, uint64_t> whoGives; // symbol -> addr

	inline bool validOffset(size_t off) { return off < sz; }

	// Steps. It's critical that they're done in order
	void parseAndLoad(); // PHDRs, fills "pages"
	void parseSections(); // SHDRs, fills "mapSections"
	void findExports(); // Exported symbols, fills "dynsyms"
	void findNeededLibs(); // Shared objects, fills "libs"
	void relocation();
	void makeRELRO();

public:
	ELF() = default;
	ELF(void* data, size_t sz)
		: data((char*)data), sz(sz)
	{}

	// Makes sure the steps are done in order. This gets called first
	inline void doit() {
		parseAndLoad();
		if(error) return;
		parseSections();
		if(error) return;
		findExports();
		if(error) return;
		findNeededLibs();
	}

	// Then this
	void give(ELF& elf, uint64_t base);

	// And finally this
	inline void finish() {
		if(error) return;
		relocation();
		// makeRELRO
	}

	inline pages_t& getPages() { return pages; }
	inline uint64_t getEntry() const { return entry; }
	inline dynsyms_t& getDynamicSymbols() { return dynsyms; }
	inline libs_t& getLibs() { return libs; }

	bool safeStringsSection(SHDR*);

	inline size_t getError() const { return error; }
};

#endif
