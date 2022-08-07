#version 450 core
in vec2 uv;
in float textureIndex;
in flat int entity;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int color2;

void main() {
	vec4 texColor = texture(u_Textures[int(textureIndex)], uv);

	color2 = entity;
	if(texColor.a <= 0.01f){
		discard;
	}else{
		fragColor = texColor;
	}
}
