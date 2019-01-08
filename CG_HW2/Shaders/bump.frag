#version 400

in vec2 tex;

out vec4 outColor;

uniform sampler2D Tex2D;

//Material
uniform vec3 mat_amb;
uniform vec3 mat_dif;
uniform vec3 mat_spe;
uniform float mat_shi;

//Light
uniform vec3 lig_pos;
uniform vec3 lig_amb;
uniform vec3 lig_dif;
uniform vec3 lig_spe;

in vec3 N;;
in vec3 FragPosition;

uniform vec3 ViewPosition;

void main()
{
	// ambient
    vec3 ambient = lig_amb * mat_amb;
  	
    // diffuse 
    vec3 norm = normalize(N);
    vec3 lightDir = normalize(lig_pos - FragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lig_dif * (diff * mat_dif);
    
    // specular
    vec3 viewDir = normalize(ViewPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat_shi);
    vec3 specular = lig_spe * (spec * mat_spe);

	// attenuation
	float attenuation = 1 * 1 / sqrt(length(lig_pos - FragPosition));
	diffuse *= attenuation;
	specular *= attenuation;

	outColor = vec4(ambient+diffuse, 1)*texture2D(Tex2D, tex) + vec4(specular,1);
	//outColor = texture2D(Tex2D, tex);
}