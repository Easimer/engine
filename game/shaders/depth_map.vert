#version 430 core
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in uint in_bone;
layout(location = 4) in uint in_mat;

layout(location = 10) uniform mat4 mat_view; // LIGHT'S VIEW MATRIX
layout(location = 11) uniform mat4 mat_proj; // LIGHT'S ORTHOGONAL PROJECTION MATRIX
layout(location = 12) uniform mat4 mat_trans; // MODEL'S TRANSFORMATION MATRIX

void main() {
    gl_Position = mat_proj * mat_view * mat_trans * vec4(in_pos, 1.0);
}  