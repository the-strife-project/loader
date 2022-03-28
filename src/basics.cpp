#include "basics.hpp"

extern "C" void* memcpy(void* dst, void* src, size_t count) {
	asm volatile("cld\nrep movsb" :: "D"(dst), "S"(src), "c"(count));
	return dst;
}

extern "C" void* memmove(void* dst, void* src, size_t count) {
	asm volatile("cld\nrep movsb" :: "D"(dst), "S"(src), "c"(count));
	return dst;
}

extern "C" void* memset(void* dst, char c, size_t count) {
	asm volatile("cld\nrep stosb" :: "D"(dst), "S"(c), "c"(count));
	return dst;
}
