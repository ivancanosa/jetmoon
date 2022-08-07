#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;
uniform vec3 colorFiltering;

uniform float contrast;
uniform float brightness;
uniform float saturation;
uniform float gamma;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = texture(u_Texture, uv);
	fragColor.rgb = clamp(contrast*(fragColor.rgb - 0.5) + 0.5 + brightness, 0., 999.);
	fragColor.rgb = fragColor.rgb * colorFiltering;
	float grayscale = fragColor.r*0.299 + fragColor.g*0.587 + fragColor.b*0.114;
	fragColor.r = clamp(grayscale*(1.-saturation) + fragColor.r*saturation, 0., 999.);
	fragColor.g = clamp(grayscale*(1.-saturation) + fragColor.g*saturation, 0., 999.);
	fragColor.b = clamp(grayscale*(1.-saturation) + fragColor.b*saturation, 0., 999.);
	fragColor.rgb = clamp(fragColor.rgb, 0., 1.); // Tonemapping
	fragColor.r = pow(fragColor.r, gamma);
	fragColor.g = pow(fragColor.g, gamma);
	fragColor.b = pow(fragColor.b, gamma);

	fragColor.a = 1.0;
}
