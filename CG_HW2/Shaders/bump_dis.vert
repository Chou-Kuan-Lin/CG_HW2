#version 400

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec3 Nor;
layout(location = 2) in vec2 Tex;

//texture
out vec2 tex;

//position
uniform mat4 P;
uniform mat4 MV;

//phone shading

void main()
{
	tex = Tex;
	gl_Position = P * MV * vec4( Pos, 1 );
}