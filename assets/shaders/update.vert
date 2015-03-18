#version 150 core

in vec3   position;
in vec4   color;
in vec3   rotation;

out vec3  oPosition;
out vec4  oColor;
out vec3  oRotation;

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

void main()
{
	oPosition = position + vec3(0.0, 0.0, 0.0);// rotateAroundX( rotation.x, rotateAroundY( rotation.y, rotateAroundZ( rotation.z, vec4(position.x, position.y, position.z, position.w))));         
	oColor = color;
	oRotation = rotation;
}