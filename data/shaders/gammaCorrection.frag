#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;
uniform float gamma;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = texture(u_Texture, uv);
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
	fragColor.a = 1.;
}
