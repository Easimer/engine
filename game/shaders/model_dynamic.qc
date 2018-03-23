// Shader for dynamic models (prop_dynamic)
$name					"model_dynamic"
$description			"Shader for dynamic models"

// Path to GLSL code
$vertex_shader			"data/shaders/model_dynamic.vert"
$fragment_shader		"data/shaders/model_dynamic.frag"

// Name of transformation, view and projection matrix uniforms
$uniform_view			"mat_view"
$uniform_trans			"mat_trans"
$uniform_proj			"mat_proj"

// Materials
$diffuse_key			"diffuse"
$diffuse_default		"data/textures/builtin/diffuse.png"
$specular_key			"specular"
$specular_default		"data/textures/builtin/specular_flat.png"
$normal_key				"normal"
$normal_default			"data/textures/builtin/normal_flat.png"
$opacity_key			"opacity"
$opacity_default		"data/textures/builtin/opacity_flat.png"

$depth_texture			"tex_depth"

// Shader is lit
$lit					"1"

// light_no_global: set to true when global lighting should not be used
$light_no_global		"light_no_global"

// Local light parameters
$lightl_pos_x			"lightl_pos_x"
$lightl_pos_y			"lightl_pos_y"
$lightl_pos_z			"lightl_pos_z"
$lightl_color_r			"lightl_color_r"
$lightl_color_g			"lightl_color_g"
$lightl_color_b			"lightl_color_b"
$lightl_color_a			"lightl_color_a"
$lightl_disabled		"lightl_disabled"

// Global light parameters
$lightg_rot_x			"lightg_rot_x"
$lightg_rot_y			"lightg_rot_y"
$lightg_rot_z			"lightg_rot_z"
$lightg_color_r			"lightg_color_r"
$lightg_color_g			"lightg_color_g"
$lightg_color_b			"lightg_color_b"
$lightg_color_a			"lightg_color_a"
$lightg_disabled		"lightg_disabled"