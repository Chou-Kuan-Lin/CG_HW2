#version 400

in vec2 tex;

out vec4 outColor;
vec4 tempColor;

uniform sampler2D Tex2D;
uniform sampler2D Tex2D_noise;
uniform float dissolveFactor;

//change color
uniform float R;
uniform float G;
uniform float B;

void main()
{
	vec4 MainTex = texture2D( Tex2D, tex);
	vec4 NoiseTex = texture2D( Tex2D_noise, tex);
	
	//cut
	if(NoiseTex.g < dissolveFactor)
		discard;
	else if(dissolveFactor+0.2222>NoiseTex.r)
		tempColor = vec4( R, B, G, 1 );
	else
		tempColor = MainTex;
		
	outColor = tempColor;
}