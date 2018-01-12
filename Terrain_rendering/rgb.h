#ifndef RGB_H
#define RGB_H

struct rgb {
	unsigned char   red;
	unsigned char green;
	unsigned char  blue;

	rgb(){}

	rgb(unsigned char _red, unsigned char _green, unsigned char  _blue) {
		red = _red; green = _green; blue = _blue;
	}
};



#endif