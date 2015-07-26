#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat4 ciViewMatrix;
uniform mat3 ciNormalMatrix;

uniform float scale;

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

const float PI = 3.1415926;

in vec4 vColor[];
in vec3 vRotation[];

out vec3 LightIntensityPerVertex;

//
// Rotation functions.
// Angles are in radians.
//

vec4 rotateAroundX( float angle, vec4 v )
{
     float sa = sin( angle );
     float ca = cos( angle );

     return vec4( v.x,
                    ca*v.y - sa*v.z,
                    sa*v.y + ca*v.z,
                    v.w );
}

//---------

vec4 rotateAroundY( float angle, vec4 v )
{
     float sa = sin( angle );
     float ca = cos( angle );

     return vec4( sa*v.z + ca*v.x,   v.y,
                    ca*v.z - sa*v.x,   v.w );
}

//---------

vec4 rotateAroundZ( float angle, vec4 v )
{
     float sa = sin( angle );
     float ca = cos( angle );

     return vec4( ca*v.x - sa*v.y,
                    sa*v.x + ca*v.y,
                    v.z, v.w );
}

vec3 getLightIntensityForVertex(vec4 pos, vec3 normal) {
	vec3 newNormal = normalize(ciNormalMatrix * normal);
	vec4 eyeVertexPosition = ciModelView * pos;
	vec4 eyeLightPosition = ciViewMatrix * vec4(Position, 1.0);
	vec3 s = normalize(vec3(eyeLightPosition - eyeVertexPosition)); // vector from vertex position to light position
	vec3 v = normalize(-vec3(eyeVertexPosition)); // vector from vertex into camera
	vec3 r = reflect( -s, newNormal ); // reflect s around newNormal to get reflected ray

	// calculate ambient light
	vec3 ambient = La * Ka;

	// calculate diffuse light
	float sDotN = max(dot(s, newNormal), 0.0);
	vec3 diffuse = Ld * Kd * sDotN;

	// calculate specular light
	vec3 specular = vec3(0.0);
	if(sDotN > 0.0) {
		specular = Ls * Ks * pow(max(dot(r,v), 0.0), Shininess);
	}

	return (ambient + diffuse + specular)/3.0;
}

void main()
{
	float scaleX = scale;
	float scaleY = scaleX;
	float scaleZ = scaleY;

	vec3 rotate = vRotation[0];
	vec4 vertex = gl_in[0].gl_Position;
	vec4 newVertex;         

	// left
	vec3 normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-1, 0, 0, 0)))).xyz; 
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal );         
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	  
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();

	// right
	normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(1, 0, 0, 0)))).xyz; 
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();

	// front
	normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(0, 0, 1, 0)))).xyz; 	
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();

	// back
	normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(0, 0, -1, 0)))).xyz; 
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();

	// top
	normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(0, 1, 0, 0)))).xyz; 
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();

	// bottom
	normal = rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(0, -1, 0, 0)))).xyz; 
	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();

	newVertex = vertex + rotateAroundX( rotate.x, rotateAroundY( rotate.y, rotateAroundZ( rotate.z, vec4(-scaleX, -scaleY, -scaleZ, 0))));         
	LightIntensityPerVertex = getLightIntensityForVertex(newVertex, normal);
	gl_Position = ciModelViewProjection * ( newVertex);
	EmitVertex();
	EndPrimitive();    
}