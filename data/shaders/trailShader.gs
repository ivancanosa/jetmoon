#version 450 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;


uniform mat4 viewTransform;


in VsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
	vec2 adjacentPoint;
	float width;
} vsOut[];

out GsOut {
	vec2 uv;
	float textureIndex;
	vec4 color;
} gsOut;

void main() {    
	gsOut.textureIndex = vsOut[0].textureIndex;

	vec2 tan0 = normalize(vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y) - vsOut[0].adjacentPoint);
	vec2 tan1 = normalize(vec2(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y) - vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y));
	vec2 tan2 = normalize(vsOut[1].adjacentPoint - vec2(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y));

	vec2 norm0 = vec2(-tan0.y, tan0.x);
	vec2 norm1 = vec2(-tan1.y, tan1.x);
	vec2 norm2 = vec2(-tan2.y, tan2.x);

	vec2 normal0 = normalize(norm0 + norm1);
	vec2 normal1 = normalize(norm1 + norm2);

	float width0 = vsOut[0].width/2;
	float width1 = vsOut[1].width/2;

	gsOut.color = vsOut[0].color;
	gsOut.uv = vec2(vsOut[0].uv.x, 0.);
	gl_Position = viewTransform * (gl_in[0].gl_Position - vec4(normal0*width0, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[0].uv.x, 1.);
	gl_Position = viewTransform * (gl_in[0].gl_Position + vec4(normal0*width0, 0., 0.));
	EmitVertex();
	gsOut.color = vsOut[1].color;
	gsOut.uv = vec2(vsOut[1].uv.x, 0.);
	gl_Position = viewTransform * (gl_in[1].gl_Position - vec4(normal1*width1, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[1].uv.x, 1.);
	gl_Position = viewTransform * (gl_in[1].gl_Position + vec4(normal1*width1, 0., 0.));
	EmitVertex();

	EndPrimitive();
}  
