#include "ELF/ELF.hpp"
#include "syscalls.hpp"
#include <basics.hpp>

void map(PID pid, ELF& elf, size_t id, bool copy) {
	uint64_t base = aslrGet(pid, id);

	for(auto const& x : elf.getPages()) {
		uint64_t remote = base + x.f;
		remote |= elf.getPerms()[x.f];
		uint64_t local = x.s;

		// If local is zero, then this page wasn't actually in the executable,
		//   and it's a byproduct of the behaviour of (unordered_)map.
		// I think this doesn't happen, but just in case...
		if(!local)
			continue;

		if(copy) {
			void* newlocal = std::mmap();
			memcpy(newlocal, (void*)local, PAGE_SIZE);
			local = (uint64_t)newlocal;
		}

		bool result = mapIn(pid, local, remote);

		// A fail has to be caused by a bug
		if(!result)
			*(uint64_t*)0x6969 = 0;
	}
}
