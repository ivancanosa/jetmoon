#version 450 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUV;
layout (location = 2) in float iTextureIndex;
layout (location = 3) in vec4 iTextureClip;
layout (location = 4) in int iEntity;

uniform mat4 viewTransform;

out vec2 uv;
out float textureIndex;
out flat int entity;

void main() {
	textureIndex = iTextureIndex;
	gl_Position = viewTransform * vec4(iPos, 1.0);
	uv = vec2(iTextureClip[0] + iUV.x*iTextureClip[2], iTextureClip[1] + iUV.y*iTextureClip[3]);
	entity = iEntity;
}
