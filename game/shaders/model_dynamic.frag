#version 430 core

layout(location = 20) uniform sampler2D tex_tex1;
layout(location = 21) uniform sampler2D tex_tex2;
layout(location = 22) uniform sampler2D tex_tex3;
layout(location = 23) uniform sampler2D tex_tex4;
layout(location = 24) uniform sampler2D tex_tex5;

in vec3 pos;
in vec3 normal;
in vec2 uv;
in uint bone;
in flat uint mat;

out vec4 FragColor;

void main()
{
	if(mat == 0)
		FragColor = texture2D(tex_tex1, uv);
	if(mat == 1)
		FragColor = texture2D(tex_tex2, uv);
	if(mat == 2)
		FragColor = texture2D(tex_tex3, uv);
	if(mat == 3)
		FragColor = texture2D(tex_tex4, uv);
	if(mat == 4)
		FragColor = texture2D(tex_tex5, uv);
}
