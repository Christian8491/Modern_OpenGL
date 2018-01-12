#ifndef BITMAP_FILE_HEADER_H
#define BITMAP_FILE_HEADER_H

#include "Common.h"

struct bitmap_file_header {
	unsigned short type;
	unsigned int   size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int   off_bits;

	unsigned int struct_size() const {
		return sizeof(type)+sizeof(size)+sizeof(reserved1)+sizeof(reserved2)+sizeof(off_bits);
	}

	void clear() { memset(this, 0x00, sizeof(bitmap_file_header)); }
};

#endif