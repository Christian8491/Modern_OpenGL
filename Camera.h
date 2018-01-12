#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"
#include "Vec.h"

struct Camera {

	vec4 position;
	vec4 front;
	vec4 up;

	Camera() {}
	
	void initialize() {

		position = vec4(0.0, 2.2, 0.0, 1.0);
		front = vec4(-1.5, 0.0, 3.2, 1.0);
		up = vec4(0.0, 1.0, 0.0, 1.0);
	}

	vec3 position_3() {
		vec3 aux = (position.x, position.y, position.z);
		return aux;
	}

};

#endif 
