#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;
uniform float threshold;


layout(location = 0) out vec4 fragColor;

float rgb2Light(vec4 c){
	return dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
	vec4 texColor = texture(u_Texture, uv);
	fragColor = vec4(1., 1., 1., 1.);
	if(rgb2Light(texColor) < threshold){
		fragColor = vec4(0., 0., 0., 1.);
	}
}
