$name				"model_dynamic"
$description		"Shader for dynamic models"

$vertex_shader			"data/shaders/model_dynamic.vert"
$fragment_shader		"data/shaders/model_dynamic.frag"

$uniform_view			"mat_view"
$uniform_trans			"mat_trans"
$uniform_proj			"mat_proj"

$diffuse_key			"diffuse"
$diffuse_default		"data/textures/builtin/diffuse.png"
$specular_key			"specular"
$specular_default		"data/textures/builtin/specular_flat.png"
$normal_key				"normal"
$normal_default			"data/textures/builtin/normal_flat.png"
$opacity_key			"opacity"
$opacity_default		"data/textures/builtin/opacity_flat.png"