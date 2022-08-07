#version 450 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iColor;
layout (location = 2) in vec2 iUv;
layout (location = 3) in float iIntensity;

uniform mat4 viewTransform;

out vec3 color;
out vec2 uv;
out float intensity;

void main() {
	gl_Position = viewTransform * vec4(iPos, 1.0);
	color = iColor;
	uv = iUv;
	intensity = iIntensity;
}
