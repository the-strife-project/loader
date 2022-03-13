#include "ELF/ELF.hpp"
#include <PageAlloc/PageAlloc.hpp>
#include <utility>
#include <syscalls>
#include "syscalls/syscalls.hpp"

extern "C" void _start(void* stdlibptr, size_t stdlibsz) {
	// Loader starts loading stdlib and caching it
	ELF stdlib(stdlibptr, stdlibsz);
	stdlib.parseAndLoad();

	// Only the first time :p
	bool doFree = false;
	ELF elf(stdlib);

	// Keep receiving programs from the kernel and returning maps
	while(true) {
		/*
			Send to the kernel:
			- Error
			- Entry point

			Receive:
			- Size of the new ELF at stdlibptr
		*/
		size_t sz = backFromLoader(elf.howItWent(), elf.getEntry());
		if(sz){}

		// Should free previous ("ELF")?
		if(doFree) {
			//
		} else {
			doFree = true;
		}

		// Parse ELF

		// New process
		PID pid = makeProcess();
		if(pid){}

		// MapIns

		*(uint8_t*)0 = 0;
	}

	/*asm volatile("syscall"
	  :: "a" (0));*/
	while(true);
}
