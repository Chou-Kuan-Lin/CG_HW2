#version 400

in vec2 tex;
in vec3 N;
in vec3 FragPosition;

out vec4 outColor;

uniform sampler2D Tex2D;
uniform sampler2D Tex2D_ramp;

//Material
uniform vec3 mat_dif;

//Light
uniform vec3 lig_pos;
uniform vec3 lig_dif;

void main()
{
	vec3 norm = normalize(N);
    vec3 lightDir = normalize(lig_pos - FragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lig_dif * (diff * mat_dif);
		
	// attenuation
	diffuse *= 1 * 1 / sqrt( length(lig_pos - FragPosition) );
	
	float index = diffuse.g;
	if( index >= 0.9 )
		index = 0.9;
	else if( index <= 0.1 )
		index = 0.1;
	
	vec2 bw = vec2( index, index );
	vec4 ff = texture2D( Tex2D_ramp, bw);
	outColor = texture2D( Tex2D, tex ) * ff;
}