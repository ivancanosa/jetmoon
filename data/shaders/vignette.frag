#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float intensity;
uniform float innerRadius;
uniform float outerRadius;

layout(location = 0) out vec4 fragColor;



void main() {
	vec4 color = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
	vec2 resolution = textureSize(u_Texture, 0);
	vec2 relativePosition = gl_FragCoord.xy / resolution - 0.5;
	float len = length(relativePosition);
	float vignette = smoothstep(outerRadius, innerRadius, len);
	color.rgb = mix(color.rgb, color.rgb * vignette, intensity);

	fragColor = color;
}
