/* basic Phong shader: Ambient + Specular + Diffuse */

#version 330

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

layout (std140) uniform Light{
	vec4 Position;
	vec3 La;	// ambient light intensity
	vec3 Ld;	// diffuse light intensity
	vec3 Ls;	// specular light intensity
};

layout (std140) uniform Material {
	vec3 Ka;	// ambient reflectivity
	vec3 Kd;	// diffuse reflectivity
	vec3 Ks;	// specular reflectivity
	float Shininess;	// specular shininess
};

in vec4	ciPosition;
in vec2	ciTexCoord0;
in vec3	ciNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 LightIntensity;

void main( void )
{
	TexCoord = ciTexCoord0;
	Normal = ciNormalMatrix * ciNormal;
	gl_Position	= ciModelViewProjection * ciPosition;	

	vec4 eyeCoords = ciModelView * ciPosition;

	LightIntensity = Material.Kd;
}
