#include "ELF.hpp"

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
