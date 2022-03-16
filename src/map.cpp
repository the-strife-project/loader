#include "ELF/ELF.hpp"
#include "syscalls.hpp"

void map(PID pid, ELF& elf, size_t id) {
	uint64_t base = aslrGet(pid, id);

	for(auto const& x : elf.getPages()) {
		uint64_t remote = base + x.f;
		uint64_t local = x.s;

		// TODO permissions
		bool result = mapIn(pid, local, remote | MAP_IN_WRITE | MAP_IN_EXEC);

		// A fail has to be caused by a bug
		if(!result)
			*(uint64_t*)0x6969 = 0;
	}
}
