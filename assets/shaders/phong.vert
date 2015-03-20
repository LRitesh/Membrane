/* basic Per Vertex Phong shader: Ambient + Specular + Diffuse */
/* reference: GLSL CookBook (Chapter 2) */

#version 330

in vec4	ciPosition;

in vec3 iPosition;
in vec4 iColor;
in vec3 iRotation;

out vec3 vRotation;
out vec4 vColor;

void main( void )
{
	vColor = iColor; 	
	vRotation = iRotation;	
	gl_Position	= ciPosition;	
}
