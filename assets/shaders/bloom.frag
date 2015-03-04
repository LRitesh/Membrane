#version 330

uniform sampler2D renderedTexture;

in vec4	Color;
in vec3	Normal;
in vec2	TexCoord;

float map(float value, float min1, float max1, float min2, float max2);

void main( void )
{
	vec4 sum = vec4(0);  
	vec4 source = texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y));
	
	float blurSize = 1.0 / 512.0;

	sum += texture2D(renderedTexture, vec2(TexCoord.x - 4.0*blurSize, TexCoord.y)) * 0.05;
	sum += texture2D(renderedTexture, vec2(TexCoord.x - 3.0*blurSize, TexCoord.y)) * 0.09;
	sum += texture2D(renderedTexture, vec2(TexCoord.x - 2.0*blurSize, TexCoord.y)) * 0.12;
	sum += texture2D(renderedTexture, vec2(TexCoord.x - blurSize, TexCoord.y)) * 0.15;
	sum += source * 0.16;
	sum += texture2D(renderedTexture, vec2(TexCoord.x + blurSize, TexCoord.y)) * 0.15;
	sum += texture2D(renderedTexture, vec2(TexCoord.x + 2.0*blurSize, TexCoord.y)) * 0.12;
	sum += texture2D(renderedTexture, vec2(TexCoord.x + 3.0*blurSize, TexCoord.y)) * 0.09;
	sum += texture2D(renderedTexture, vec2(TexCoord.x + 4.0*blurSize, TexCoord.y)) * 0.05;

	// blur in y (vertical)
	// take nine samples, with the distance blurSize between them
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y - 4.0*blurSize)) * 0.05;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y - 3.0*blurSize)) * 0.09;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y - 2.0*blurSize)) * 0.12;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y - blurSize)) * 0.15;
	sum += source * 0.16;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y + blurSize)) * 0.15;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y + 2.0*blurSize)) * 0.12;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y + 3.0*blurSize)) * 0.09;
	sum += texture2D(renderedTexture, vec2(TexCoord.x, TexCoord.y + 4.0*blurSize)) * 0.05;

	gl_FragColor = sum * 1.2 + texture2D(renderedTexture, TexCoord);
}

float map(float value, float min1, float max1, float min2, float max2)
{
	if(min1 == max1)
	{
		return value; 
	}	
	else
	{
		return (((value - min1) / (max1 - min1)) * (max2 - min2) + min2);
	}
}