#version 450 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUV;

out vec2 uv;

void main() {
	gl_Position = vec4(iPos, 1.0);
	uv = iUV;
}
