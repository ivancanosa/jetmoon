#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in VsOut {
	vec2 uv;
	float textureIndex;
	int entity;
	float tilingFactor;
	vec4 color;
} vsOut[];

out vec2 uv;
out float textureIndex;
flat out int entity;
flat out float tilingFactor;
out vec4 color;


void main() {    
	for(int i=0; i<3; i++){
		gl_Position = gl_in[i].gl_Position;
		uv = vsOut[i].uv;
		textureIndex = vsOut[i].textureIndex;
		entity = vsOut[i].entity;
		tilingFactor = vsOut[i].tilingFactor;
		color = vsOut[i].color;
		EmitVertex();
	}
    
    EndPrimitive();
}  
