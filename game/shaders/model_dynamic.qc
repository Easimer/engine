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

// Shader is lit
$lit					"1"

$light_pos_x			"light_pos_x"
$light_pos_y			"light_pos_y"
$light_pos_z			"light_pos_z"
$light_color_r			"light_color_r"
$light_color_g			"light_color_g"
$light_color_b			"light_color_b"
$light_color_a			"light_color_a"
