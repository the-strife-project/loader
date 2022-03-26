#include "ELF.hpp"

bool ELF::validRegion(size_t off, size_t sz) {
	/*
		Here's the deal: if there's a malicious size in the ELF,
		it could overflow the offset so that it points to a valid address.
		This is why checking (off+sz-1) to be in-bounds is not enough.
		Naive solution: iterate through all pages checking that it's cool.
		A better one: I want to prevent overflow (carry in unsigned). So the
		actual question is: does it carry? Only if MSB is 1.
	*/

	if(off & (1ull << 63))
		return false;
	if(sz & (1ull << 63))
		return false;

	// It does not overflow

	if(sz)
		return validOffset(off + sz - 1);
	else
		return validOffset(off);
}

// Assumes 's' is safe to dereference!
bool ELF::safeStringsSection(SHDR* s) {
	// Non-zero size?
	if(!(s->sh_size))
		return false;

	// Null-terminated section?
	if(*(data + s->sh_offset + s->sh_size - 1) != 0)
		return false;

	// At this point, all in-bounds starting string is in-bounds ending
	// Complicated, I know
	return true;
}
