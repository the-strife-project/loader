#ifndef BASICS_HPP
#define BASICS_HPP

inline void memcpy(void* dst, void* src, size_t count) {
	asm volatile("cld\nrep movsb" :: "D"(dst), "S"(src), "c"(count));
}

inline void memset(void* dst, char c, size_t count) {
	asm volatile("cld\nrep stosb" :: "D"(dst), "S"(c), "c"(count));
}

#endif
