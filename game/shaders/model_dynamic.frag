#version 400 core


in vec3 pos;
in vec3 normal;
in vec2 uv;
in uint bone;
in uint mat;

out vec3 FragColor;

void main()
{
    FragColor = vec3(uv.x, uv.y, 1);
}
