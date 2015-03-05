#version 330

uniform vec4 color;

in vec3	Normal;
in vec2	TexCoord;
in vec3 LightIntensity;

void main( void )
{
	vec4 t = color;
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	gl_FragColor = vec4(LightIntensity, 1.0);
}