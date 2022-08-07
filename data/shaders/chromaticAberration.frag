#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float rOffset;
uniform float gOffset;
uniform float bOffset;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 rValue = texture(u_Texture, vec2(uv.x - rOffset, uv.y));  
    vec4 gValue = texture(u_Texture, vec2(uv.x - gOffset, uv.y));
    vec4 bValue = texture(u_Texture, vec2(uv.x - bOffset, uv.y));  
	ivec2 originPos = ivec2(gl_FragCoord.xy);
    fragColor = vec4(rValue.r, gValue.g, bValue.b, 1.0);
	fragColor.a = 1.0;
}
