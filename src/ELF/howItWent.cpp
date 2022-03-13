#include "ELF.hpp"

// Turn the bools into an error code
size_t ELF::howItWent() const {
	size_t ret = 0;

	if(!isELF) ret |= 1 << 0;
	if(!is64) ret |= 1 << 1;
	if(!isLE) ret |= 1 << 2;
	if(!isArch) ret |= 1 << 3;
	if(invalidOffset) ret |= 1 << 4;
	if(noPHDRs) ret |= 1 << 5;

	return ret;
}
