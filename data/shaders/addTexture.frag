#version 450 core
in vec2 uv;


uniform float intensity;

layout(location = 0) out vec4 fragColor;
layout (binding = 0) uniform sampler2D u_Textures[32];

void main() {
	vec4 texColor0 = texture(u_Textures[0], uv);
	vec4 texColor1 = texture(u_Textures[1], uv);
	fragColor = texColor0 + intensity*texColor1;
	fragColor.a = 1.0;
}
