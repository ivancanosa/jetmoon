#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float intensity;
uniform vec4 color;

layout(location = 0) out vec4 fragColor;

void main() {
	vec4 textureColor = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
	fragColor = mix(textureColor, color, intensity);
	fragColor.a = 1.0;
}
