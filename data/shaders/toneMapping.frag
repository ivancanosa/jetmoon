#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;
uniform float exposure;
uniform float gamma;

layout(location = 0) out vec4 fragColor;

float rgb2Light(vec3 c){
	return dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
	vec3 hdrColor = texture(u_Texture, uv).rgb;
//	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
//	vec3 mapped = hdrColor*(1+(hdrColor/(exposure)))/(1+hdrColor);
	float lum_in = rgb2Light(hdrColor);
	float lum_out = lum_in*(1+(lum_in/(exposure*exposure)))/(1+lum_in);
	vec3 mapped = hdrColor*(lum_out/lum_in);
	mapped = pow(mapped, vec3(1.0 / gamma));
	fragColor = vec4(mapped.rgb, 1.);
}
