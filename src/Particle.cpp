#include "Particle.h"

#include "cinder/Rand.h"

Particle::Particle()
{
	mPosition = ci::randVec3f() * 15.0f;//glm::vec3( 0.0f, 0.0f, 0.0f ); //
	mRotation = ci::randVec3f();
	mColor = glm::vec4( 0.3, 0.7, 0.0, 1.0f ); // ci::randFloat(0.3f, 1.0f) );
}
