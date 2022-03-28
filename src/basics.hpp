#ifndef BASICS_HPP
#define BASICS_HPP

#include <types>

extern "C" void* memcpy(void* dst, void* src, size_t count);
extern "C" void* memmove(void* dst, void* src, size_t count);
extern "C" void* memset(void* dst, char c, size_t count);

#endif
