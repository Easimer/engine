#version 430 core

layout(location = 20) uniform sampler2D tex_diffuse;
layout(location = 21) uniform sampler2D tex_normal;
layout(location = 22) uniform sampler2D tex_specular;
layout(location = 23) uniform sampler2D tex_opacity;
layout(location = 24) uniform sampler2D tex_tex5;

in vec3 pos;
in vec3 normal;
in vec2 uv;
in uint bone;
in flat uint mat;

out vec4 FragColor;

float lambert(vec3 N, vec3 L)
{
  vec3 n = normalize(N);
  vec3 l = normalize(L);
  float res = dot(n, l);
  return max(res, 0.0);
}

void main()
{
	float res = lambert(normal, vec3(0, 0, -100));
	FragColor = vec4(res, res, res, 1.0);
	
}
