#version 330

in vec4	Color;
in vec3	Normal;
in vec2	TexCoord;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	gl_FragColor = vec4(0.3, 0.7, 0.0, 0.8) * diffuse;
}