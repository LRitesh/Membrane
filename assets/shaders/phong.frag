#version 330

in vec3 LightIntensityPerVertex;

void main( void )
{
	gl_FragColor = vec4(LightIntensityPerVertex, 1.0);
}