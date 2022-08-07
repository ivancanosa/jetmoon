#version 450 core
in vec2 uv;

uniform vec3 backgroundColor;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 fragColor;

void main() {
	vec4 backgroundColor2 = vec4(backgroundColor, 1.);
	vec4 texColor0 = texture(u_Textures[0], uv);
	vec4 texColor1 = texture(u_Textures[1], uv);

	vec4 maskColor = max(texColor1, backgroundColor2);
	fragColor = texColor0 * maskColor + texColor1/2.;
	fragColor.a = 1.0;
}
