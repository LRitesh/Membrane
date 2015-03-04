#version 330

uniform vec4 color;

in vec3	Normal;
in vec2	TexCoord;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	gl_FragColor = color * diffuse;
}