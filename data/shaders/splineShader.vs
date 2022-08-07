#version 450 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec4 iColor;
layout (location = 2) in vec2 iUV;
layout (location = 3) in float despl;
layout (location = 4) in vec4 iCoefficientsX;
layout (location = 5) in vec4 iCoefficientsY;
layout (location = 6) in float iThickness;
layout (location = 7) in float iTextureIndex;
layout (location = 8) in float iTilingFactor;
layout (location = 9) in int iEntity;


uniform mat4 viewTransform;

out VsOut {
	vec4 color;
	vec2 uv;
	float despl;
	vec4 coefficientsX;
	vec4 coefficientsY;
	float thickness;
	float textureIndex;
	float tilingFactor;
	int entity;
} vsOut;


void main() {
	gl_Position = vec4(iPos, 1.0);
	vsOut.color = iColor;
	vsOut.uv = iUV;
	vsOut.despl = despl;
	vsOut.coefficientsX = iCoefficientsX;
	vsOut.coefficientsY = iCoefficientsY;
	vsOut.thickness = iThickness;
	vsOut.textureIndex = iTextureIndex;
	vsOut.tilingFactor = iTilingFactor;
	vsOut.entity = iEntity;
}
