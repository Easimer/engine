#version 430 core

layout(location = 20) uniform sampler2D tex_diffuse;
layout(location = 21) uniform sampler2D tex_normal;
layout(location = 22) uniform sampler2D tex_specular;
layout(location = 23) uniform sampler2D tex_opacity;

in vec3 pos;
in vec3 normal;
in vec2 uv;
in uint bone;
in flat uint mat;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(tex_diffuse, uv);
}
