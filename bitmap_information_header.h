#ifndef BITMAP_INFORMATION_HEADER_H
#define BITMAP_INFORMATION_HEADER_H

#include "Common.h"

struct bitmap_information_header {

	unsigned int   size;
	unsigned int   width;
	unsigned int   height;
	unsigned short planes;
	unsigned short bit_count;
	unsigned int   compression;
	unsigned int   size_image;
	unsigned int   x_pels_per_meter;
	unsigned int   y_pels_per_meter;
	unsigned int   clr_used;
	unsigned int   clr_important;

	unsigned int struct_size() const {
		return sizeof(size)+sizeof(width)+sizeof(height)+sizeof(planes)+sizeof(bit_count)+sizeof(compression)+
			sizeof(size_image)+sizeof(x_pels_per_meter)+sizeof(y_pels_per_meter)+sizeof(clr_used)+sizeof(clr_important);
	}

	void clear() {
		memset(this, 0x00, sizeof(bitmap_information_header));
	}

};

#endif