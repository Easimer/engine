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
const int SLT_DISABLED = 2;

uniform bool lightg_disabled;
uniform float lightg_color_r;
uniform float lightg_color_g;
uniform float lightg_color_b;
uniform float lightg_color_a;
uniform float lightg_rot_x;
uniform float lightg_rot_y;
uniform float lightg_rot_z;

uniform bool lightl_disabled;
uniform float lightl_color_r;
uniform float lightl_color_g;
uniform float lightl_color_b;
uniform float lightl_color_a;
uniform float lightl_pos_x;
uniform float lightl_pos_y;
uniform float lightl_pos_z;

uniform float game_time;

uniform bool bDebugDrawNormalsOnly;

float fog(const float distance, const float amount) {
	const float log2 = -1.442695;
	float d = distance * amount;
	return 1.0 - clamp(exp2(d * d * log2), 0.0, 1.0);
}

vec3 light_get_local(vec3 light_pos, vec3 light_color, vec3 frag_pos, vec3 frag_normal) {
	vec3 ambient = 0.1 * light_color;
	vec3 lightDir = normalize(light_pos - frag_pos);
	float diff = clamp(dot(frag_normal, lightDir), 0.0, 1.0);
	vec3 diffuse = diff * light_color;
	return (ambient + diffuse);
}

vec3 light_get_global(vec3 light_rot, vec3 light_color, vec3 frag_pos, vec3 frag_normal) {
	vec3 ambient = 0.1 * light_color;
	float diff = clamp(dot(frag_normal, light_rot), 0.0, 1.0);
	vec3 diffuse = diff * light_color;
	return (ambient + diffuse);
}

void main() {
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
	vec3 lightl_pos = vec3(lightl_pos_x, lightl_pos_y, lightl_pos_z);
	vec3 lightl_col = vec3(lightl_color_r, lightl_color_g, lightl_color_b);
	vec3 lightg_rot = normalize(vec3(lightg_rot_x, lightg_rot_y, lightg_rot_z));
	vec3 lightg_col = vec3(lightg_color_r, lightg_color_g, lightg_color_b);

	// Local light
	vec3 light_col_total = vec3(0.0, 0.0, 0.0);
	if(!lightl_disabled)
		light_col_total += light_get_local(lightl_pos, lightl_col, frag_pos, norm);
	if(!lightg_disabled)
		light_col_total += light_get_global(lightg_rot, lightg_col, frag_pos, norm);

	FragColor = vec4(light_col_total * vec3(texture(tex_diffuse, uv)), 1.0);
}
