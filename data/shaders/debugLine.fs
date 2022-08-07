#version 450 core
in vec4 color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int color2;

void main() {
	fragColor = color;
	color2 = 0;
}
