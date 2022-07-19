#include "ELF/ELF.hpp"
#include <utility>
#include <syscalls>
#include "syscalls.hpp"

#define DO_COPY true
void map(std::PID pid, ELF& elf, size_t id, bool copy=false);

size_t loadedCounter = 0;

extern "C" void _start(void* ptr, size_t stdlibsz) {
	// This loader starts loading stdlib and caching it
	// There would be no need to copy the whole ELF, but... Read "Careful now".
	void* rawstdlib = std::mmap(NPAGES(stdlibsz));
	memcpy(rawstdlib, ptr, stdlibsz);

	ELF stdlib(rawstdlib, stdlibsz);
	stdlib.doit();
	// The following line is NOT actually used, it's just for relocate() to
	//   not fail in case the "Careful now" scenario plays.
	stdlib.give(stdlib, 0);
	stdlib.finish();

	// Send stdlib parse status and get size of new ELF @ ptr (it's overwritten)
	++loadedCounter;
	size_t sz = backFromLoader(0, stdlib.getError(), 0);

	while(true) {
		std::PID lastPID = 0;
		uint64_t lastEntry = 0;

		ELF elf(ptr, sz);
		elf.doit();
		if(elf.getError()) {
			sz = backFromLoader(0, elf.getError(), 0);
			continue;
		}

		// Need to create it now so ASLR can do its thing
		lastPID = makeProcess();

		// Tell me, "elf", what do you need?
		size_t id = 1;
		for(auto const& x : elf.getLibs()) {
			if(x != "libstd.so") {
				// TODO (clearly)
				HALT_AND_CATCH_FIRE();
			}

			ELF& thelib = stdlib;

			// Note for future me: when more libraries are supported, make sure
			// there's a <set> for those loaded so, in case it's done recursively,
			// none are loaded twice. Give it a thought.
			elf.give(thelib, aslrGet(lastPID, id));

			// Careful now: I've seen some ELFs that have a local GOT section.
			// That is, a GOT for local functions. At the time I write this,
			//   stdlib is one of them. In this case, it's necessary to
			//   give() the library to itself. Weird stuff. This can't be done
			//   before because ASLR hasn't set a region for it to be loaded at.
			// It can be done multiple times though, specially in the stdlib
			//   which is always loaded. It will just overwrite the pointers.
			thelib.give(thelib, aslrGet(lastPID, id));
			// Now, relocate it
			thelib.finish();

			map(lastPID, thelib, id, DO_COPY);

			++id;
		}

		elf.finish();

		lastEntry = aslrGet(lastPID, 0) + elf.getEntry();
		map(lastPID, elf, 0);

		// That's it
		++loadedCounter;
		sz = backFromLoader(lastPID, elf.getError(), lastEntry);
	}
}
