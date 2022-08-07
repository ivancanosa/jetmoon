#version 450 core

in GsOut {
	vec4 color;
	vec2 uv;
	float textureIndex;
	float tilingFactor;
	flat int entity;
} gsOut;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int color2;

void main() {
	vec4 texColor = gsOut.color;
	texColor *= texture(u_Textures[int(gsOut.textureIndex)], gsOut.uv  * gsOut.tilingFactor);

	color2 = gsOut.entity;
	if(texColor.a <= 0.01f){
		discard;
	}else{
		fragColor = texColor;
	}
}
