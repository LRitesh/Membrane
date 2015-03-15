/* basic Per Vertex Phong shader: Ambient + Specular + Diffuse */
/* reference: GLSL CookBook (Chapter 2) */

#version 330

uniform vec4 color;

in vec4	ciPosition;

void main( void )
{
	gl_Position	= ciPosition;		
}
