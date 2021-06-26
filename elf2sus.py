#!/usr/bin/env python3

import sys, struct

ELF_HDR_EP = 0x18
ELF_HDR_PHDRS_OFF = 0x20
ELF_HDR_PHDRS_ENTSIZE = 0x36
ELF_HDR_PHDRS_COUNT = 0x38

ELF_PHDR_TYPE = 0x00
ELF_PHDR_OFF = 0x08
ELF_PHDR_VADDR = 0x10
ELF_PHDR_FILESZ = 0x20
ELF_PHDR_MEMSZ = 0x28

get8bytes = lambda off, x : x[off:off+8]
get_uint64_t = lambda off, x : struct.unpack('<Q', get8bytes(off, x))[0]
get_uint32_t = lambda off, x : struct.unpack('<I', x[off:off+4])[0]
get_uint16_t = lambda off, x : struct.unpack('<H', x[off:off+2])[0]

def getPHDRS(data):
	phdrs_off = get_uint64_t(ELF_HDR_PHDRS_OFF, data)
	phdrs_entsize = get_uint16_t(ELF_HDR_PHDRS_ENTSIZE, data)
	phdrs_count = get_uint16_t(ELF_HDR_PHDRS_COUNT, data)

	phdrs_raw = data[phdrs_off:phdrs_off + phdrs_entsize*phdrs_count]
	ret = []

	while len(phdrs_raw):
		# PT_LOAD?
		if get_uint32_t(ELF_PHDR_TYPE, phdrs_raw) == 1:
			# Has to be loaded.
			ph = {}
			ph['off'] = get_uint64_t(ELF_PHDR_OFF, phdrs_raw)
			ph['vaddr'] = get_uint64_t(ELF_PHDR_VADDR, phdrs_raw)
			ph['filesz'] = get_uint64_t(ELF_PHDR_FILESZ, phdrs_raw)
			ph['memsz'] = get_uint64_t(ELF_PHDR_MEMSZ, phdrs_raw)
			ret.append(ph)

		phdrs_raw = phdrs_raw[phdrs_entsize:]

	return ret

'''
So, a SUS file is structured like this:
- Magic number (7F555355)
- Entry point (8 bytes)
- Flat binary
'''

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print('Usage:', sys.argv[0], '<ELF file>')
		exit()

	with open(sys.argv[1], 'rb') as f:
		data = f.read()

	sus = bytearray()
	sus.extend(map(ord, list('\x7FSUS')))

	# Entry point
	ep = get8bytes(ELF_HDR_EP, data)
	sus.extend(ep)

	# Phdrs?
	phdrs = getPHDRS(data)

	flatsize = max([i['vaddr']+i['memsz']-1 for i in phdrs])
	flat = bytearray([0 for i in range(flatsize)])

	for x in phdrs:
		flat[x['vaddr']:x['vaddr']+x['filesz']] = data[x['off']:x['off']+x['filesz']]
	sus.extend(flat)

	filename = sys.argv[1].replace('.elf', '')+'.sus'

	with open(filename, 'wb') as f:
		f.write(sus)
