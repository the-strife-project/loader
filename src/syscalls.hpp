#ifndef SYSCALLS_HPP
#define SYSCALLS_HPP

#include <types>
#include <syscalls>

// Syscalls only used by the loader

inline size_t backFromLoader(PID pid, size_t err, uint64_t entry) {
	size_t ret;
	asm volatile("push %%r10\n"
				 "mov %[nr10], %%r10\n"
				 "syscall\n"
				 "pop %%r10"
				 : "=a" (ret)
				 : "D" (std::Syscalls::BACK_FROM_LOADER),
				   "S" (pid),
				   "d" (err),
				   [nr10] "r" (entry)
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

inline uint64_t aslrGet(PID pid, size_t id, size_t npages=0) {
	uint64_t ret;
	asm volatile("push %%r10\n"
				 "mov %[nr10], %%r10\n"
				 "syscall\n"
				 "pop %%r10"
				 : "=a" (ret)
				 : "D" (std::Syscalls::ASLR_GET),
				   "S" (pid),
				   "d" (id),
				   [nr10] "r" (npages)
				 : SYSCALL_CLOBBER);
	return ret;
}

#define MAP_IN_WRITE (1 << 0)
#define MAP_IN_EXEC  (1 << 1)
inline bool mapIn(PID pid, uint64_t local, uint64_t remote) {
	// Protection is given in lower bits of "remote"
	uint64_t ret;
	asm volatile("push %%r10\n"
				 "mov %[nr10], %%r10\n"
				 "syscall\n"
				 "pop %%r10\n"
				 : "=a" (ret)
				 : "D" (std::Syscalls::MAP_IN),
				   "S" (pid),
				   "d" (local),
				   [nr10] "r" (remote)
				 : SYSCALL_CLOBBER);
	return ret;
}

#endif
