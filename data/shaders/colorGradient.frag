#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform vec3 color0;
uniform vec3 color1;

layout(location = 0) out vec4 fragColor;

void main() {
	vec3 texColor = mix(color0, color1, texture(u_Texture, uv).rgb);
	fragColor = vec4(texColor, 1.);
}
