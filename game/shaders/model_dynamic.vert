#version 430 core

layout(location = 10) uniform mat4 mat_view;
layout(location = 11) uniform mat4 mat_proj;
layout(location = 12) uniform mat4 mat_trans;
layout(location = 13) uniform mat4 mat_lightspace;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in uint in_bone;
layout(location = 4) in uint in_mat;

out vec3 pos;
out vec3 normal;
out vec2 uv;
out uint bone;
out flat uint mat;
out vec3 frag_pos;
out float dist;
out vec3 cam_pos;
out vec4 pos_lightspace;

void main()
{
	vec4 pos4 = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
	gl_Position = mat_proj * mat_view * mat_trans * pos4;
	
	pos = in_pos;
	normal = in_normal;
	uv = in_uv;
	bone = in_bone;
	mat = in_mat;

	frag_pos = vec3(mat_trans * pos4);
	dist = -(mat_view * pos4).z;

	// calc camera position
	mat3 rotMat = mat3(mat_view);
	vec3 d = vec3(mat_view[3]);
	cam_pos = -d * rotMat;

	// 
	pos_lightspace = mat_lightspace * vec4(frag_pos, 1.0);
}
