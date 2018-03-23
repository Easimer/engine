// Shader used to generate shadow map
$name					"depth_map"
$description			"Shader used to generate depth map"

// Path to GLSL code
$vertex_shader			"data/shaders/depth_map.vert"
$fragment_shader		"data/shaders/depth_map.frag"

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

// Shader is not lit
$lit					"0"