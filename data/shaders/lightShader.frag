#version 450 core

in vec2 uv;
in vec3 color;
in float intensity;

layout(location = 0) out vec4 fragColor;

void main() {
	float v = length(uv - 0.5);
	float len = clamp(1. - v*2., 0., 1.);
	len = len * len;
	fragColor = vec4(color * len * intensity, 1.);
}
