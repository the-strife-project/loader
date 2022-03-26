#include "ELF/ELF.hpp"
#include <PageAlloc/PageAlloc.hpp>
#include <utility>
#include <syscalls>
#include "syscalls.hpp"

#define DO_COPY true
void map(PID pid, ELF& elf, size_t id, bool copy=false);

extern "C" void _start(void* ptr, size_t stdlibsz) {
	// Loader starts loading stdlib and caching it
	ELF stdlib(ptr, stdlibsz);
	stdlib.doit();
	stdlib.finish();

	// Only the first time :p
	bool doFree = false;
	ELF elf = stdlib;

	// Keep receiving programs from the kernel and returning maps
	PID lastPID = 0;
	uint64_t lastEntry = 0;
	while(true) {
		size_t sz = backFromLoader(lastPID, elf.getError(), lastEntry);
		lastPID = 0;
		lastEntry = 0;

		// Should free previous ("elf")?
		if(doFree) {
			// TODO (clearly)
			*(uint64_t*)0x69 = 0;
			while(true);
		} else {
			doFree = true;
		}

		elf = ELF(ptr, sz);
		elf.doit();
		if(elf.getError()) continue; // Any errors?

		// Need to create it now so ASLR can do its thing
		lastPID = makeProcess();

		// Tell me, "elf", what do you need?
		size_t id = 1;
		for(auto const& x : elf.getLibs()) {
			if(x != "libstd.so") {
				// TODO (clearly)
				*(uint64_t*)0x420 = 0;
			}

			// Note for future me: when more libraries are supported, make sure
			// there's a <set> for those loaded so, in case it's done recursively,
			// none are loaded twice. Give it a thought.
			elf.give(stdlib, aslrGet(lastPID, id));

			map(lastPID, stdlib, id, DO_COPY);

			++id;
		}

		elf.finish();

		lastEntry = aslrGet(lastPID, 0) + elf.getEntry();
		map(lastPID, elf, 0);

		// TODO: RELRO
	}
}
