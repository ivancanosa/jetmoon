#version 450 core

in GsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
} gsOut;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int color2;

void main() {
	vec4 texColor = gsOut.color;
	texColor *= texture(u_Textures[int(gsOut.textureIndex)], gsOut.uv);

	color2 = 0;
	if(texColor.a <= 0.01f){
		discard;
	}else{
		fragColor = texColor;
	}
}
