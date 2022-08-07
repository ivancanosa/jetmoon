#version 450 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec4 iColor;

uniform mat4 viewTransform;

out vec4 color;

void main() {
	gl_Position = viewTransform * vec4(iPos, 1.0);
	color = iColor;
}
