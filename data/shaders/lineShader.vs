#version 450 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iUv;
layout (location = 2) in float iTextureIndex;
layout (location = 3) in vec4 iColor;
layout (location = 4) in float iWidth;

out VsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
	float width;
} vsOut;

uniform mat4 viewTransform;

void main() {
	gl_Position = vec4(iPosition, 1.0);
	vsOut.uv = iUv;
	vsOut.textureIndex = iTextureIndex;
	vsOut.color = iColor;
	vsOut.width = iWidth;
}
