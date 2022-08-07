#version 450 core

in GsOut {
	vec2 uv;
	vec4 color;
	float textureIndex;
} gsOut;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int color2;

float toGrayScale(vec3 color){
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 *color.b; 
}

vec4 mergeColors(vec4 c1, vec4 c2){
	float alpha = min(c1.w, c2.w);
	float scale = toGrayScale(c1.xyz);
	return vec4(scale * c2.r, scale * c2.g, scale * c2.b, alpha);
}

void main() {
	ivec2 size = textureSize(u_Textures[int(gsOut.textureIndex)], 0);
	vec4 texColor = texelFetch(u_Textures[int(gsOut.textureIndex)], ivec2(gsOut.uv.xy * size), 0);

	texColor = mergeColors(texColor, gsOut.color);
	color2 = 0;
	if(texColor.a <= 0.01f){
		discard;
	}else{
		fragColor = texColor;
	}
}
