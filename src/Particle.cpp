#include "Particle.h"

#include "cinder/Rand.h"

Particle::Particle()
{
	mPosition = ci::randVec3f() * 20.0f ;
	mRotation = ci::randVec3f();
	mColor = glm::vec4( 0.3, 0.7, 0.0, ci::randFloat( 0.3f, 1.0f ) );
}
