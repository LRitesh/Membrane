/* basic Per Vertex Phong shader: Ambient + Specular + Diffuse */
/* reference: GLSL CookBook (Chapter 2) */

#version 330

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat4 ciViewMatrix;
uniform mat3 ciNormalMatrix;

layout (std140) uniform Light{
	vec3 Position;
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

uniform vec4 color;

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

	vec4 eyeVertexPosition = ciModelView * ciPosition;
	vec4 eyeLightPosition = ciViewMatrix * vec4(Light.Position, 1.0);
	vec3 s = normalize(vec3(eyeLightPosition - eyeVertexPosition)); // vector from vertex position to light position
	vec3 v = normalize(-vec3(eyeVertexPosition)); // vector from vertex into camera
	vec3 r = reflect( -s, Normal ); // reflect s around normal to get reflected ray

	// calculate ambient light
	vec3 ambient = Light.La * Material.Ka;

	// calculate diffuse light
	float sDotN = max(dot(s, Normal), 0.0);
	vec3 diffuse = Light.Ld * Material.Kd * sDotN;

	// calculate specular light
	vec3 specular = vec3(0.0);
	if(sDotN > 0.0) {
		specular = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
	}

	LightIntensity = ambient + diffuse + specular;
	gl_Position	= ciModelViewProjection * ciPosition;		
}
