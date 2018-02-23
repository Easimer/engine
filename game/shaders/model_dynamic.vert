#version 330 core

layout(location = 10) uniform mat4 mat_view;
layout(location = 11) uniform mat4 mat_proj;
layout(location = 12) uniform mat4 mat_trans;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in uint in_bone;
layout(location = 4) in uint in_mat;

out vec3 pos;
out vec3 normal;
out vec2 uv;
out uint bone;
out uint mat;

void main()
{
	gl_Position = mat_proj * mat_view * mat_trans * vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
	
	pos = in_pos;
	normal = in_normal;
	uv = in_uv;
	bone = in_bone;
	mat = in_mat;
}
