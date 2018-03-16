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
in vec3 frag_pos;
in float dist;

out vec4 FragColor;

const float NORM_MAX_DIFF = 0.785398163;

layout(location = 30) uniform struct LightSource {
	int type;
	vec3 position;
	float constant, linear, quadratic;
	vec4 color;
} light;

uniform float light_color_r;
uniform float light_color_g;
uniform float light_color_b;
uniform float light_color_a;
uniform float light_pos_x;
uniform float light_pos_y;
uniform float light_pos_z;

uniform float game_time;

uniform bool bDebugDrawNormalsOnly;

float fog(const float distance, const float amount) {
	const float log2 = -1.442695;
	float d = distance * amount;
	return 1.0 - clamp(exp2(d * d * log2), 0.0, 1.0);
}

void main()
{
	vec3 norm = normalize(normal);

	// Fog color
	vec3 distcol = vec3(1.0, 1.0, 1.0);

	// Texture color
	vec3 texcol = vec3(texture(tex_diffuse,uv));

	// Get normal vector
	//vec3 normal_tex = normalize(vec3(texture(tex_normal, uv)));
	//if(dot(normal_tex, norm) < NORM_MAX_DIFF) {
	//	norm = normal_tex;
	//}

	if(bDebugDrawNormalsOnly) {
		FragColor = vec4(norm, 1.0);
		return;
	}

	// Lighting
	vec3 light_pos = vec3(light_pos_x, light_pos_y, light_pos_z);
	vec3 light_col = vec3(light_color_r, light_color_g, light_color_b);

	// Ambient
	vec3 ambient = 0.1 * light_col;

	// Diffuse
	vec3 lightDir = normalize(light_pos - frag_pos);
	float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
	vec3 diffuse = diff * light_col;

	FragColor = vec4((ambient + diffuse) * vec3(texture(tex_diffuse, uv)), 1.0);
}
