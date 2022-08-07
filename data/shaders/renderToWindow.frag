#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = texture(u_Texture, uv);
}
