#version 450 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 viewTransform;

in VsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
	float width;
} vsOut[];

out GsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
} gsOut;

void main() {    
	gsOut.textureIndex = vsOut[0].textureIndex;

	vec2 tan = normalize(vec2(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y) - vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y));
	vec2 normal = vec2(-tan.y, tan.x);

	float width0 = vsOut[0].width/2;
	float width1 = vsOut[1].width/2;

	gsOut.color = vsOut[0].color;
	gsOut.uv = vec2(vsOut[0].uv.x, 0.);
	gl_Position = viewTransform * (gl_in[0].gl_Position - vec4(normal*width0, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[0].uv.x, 1.);
	gl_Position = viewTransform * (gl_in[0].gl_Position + vec4(normal*width0, 0., 0.));
	EmitVertex();
	gsOut.color = vsOut[1].color;
	gsOut.uv = vec2(vsOut[1].uv.x, 0.);
	gl_Position = viewTransform * (gl_in[1].gl_Position - vec4(normal*width1, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[1].uv.x, 1.);
	gl_Position = viewTransform * (gl_in[1].gl_Position + vec4(normal*width1, 0., 0.));
	EmitVertex();

	EndPrimitive();
}  
