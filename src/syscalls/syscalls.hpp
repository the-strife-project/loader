#ifndef SYSCALLS_HPP
#define SYSCALLS_HPP

#include <types>

// Syscalls only used by the loader

inline size_t backFromLoader(size_t err, uint64_t entry) {
	size_t ret;
	asm volatile("syscall"
				 : "=a" (ret)
				 : "D" (std::Syscalls::BACK_FROM_LOADER),
				   "S" (err),
				   "d" (entry)
				 : SYSCALL_CLOBBER);
	return ret;
}

inline PID makeProcess() {
	PID ret;
	asm volatile("syscall"
				 : "=a" (ret)
				 : "D" (std::Syscalls::MAKE_PROCESS)
				 : SYSCALL_CLOBBER);
	return ret;
}

#endif
