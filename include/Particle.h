#include "cinder\CinderGlm.h"

#pragma once
class Particle {
  public:
	Particle();
	~Particle() {};

	glm::vec3 mPosition;
	glm::vec4 mColor;
	glm::vec3 mRotation;
};

