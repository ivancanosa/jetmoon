#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 124) out;

uniform mat4 viewTransform;
const int segments = 30;

in VsOut {
	vec4 color;
	vec2 uv;
	float despl;
	vec4 coefficientsX;
	vec4 coefficientsY;
	float thickness;
	float textureIndex;
	float tilingFactor;
	int entity;
} vsOut[];


out GsOut {
	vec4 color;
	vec2 uv;
	float textureIndex;
	float tilingFactor;
	flat int entity;
} gsOut;


float evaluateX(float x){
	x += vsOut[0].despl;
	return vsOut[0].coefficientsX[0]*x*x*x + 
		vsOut[0].coefficientsX[1]*x*x + 
		vsOut[0].coefficientsX[2]*x + 
		vsOut[0].coefficientsX[3];
}
float evaluateY(float x){
	x += vsOut[0].despl;
	return vsOut[0].coefficientsY[0]*x*x*x + 
		vsOut[0].coefficientsY[1]*x*x + 
		vsOut[0].coefficientsY[2]*x + 
		vsOut[0].coefficientsY[3];
}
float derivativeX(float x){
	x += vsOut[0].despl;
	return 3*vsOut[0].coefficientsX[0]*x*x + 
		2*vsOut[0].coefficientsX[1]*x + 
		vsOut[0].coefficientsX[2];
}
float derivativeY(float x){
	x += vsOut[0].despl;
	return 3*vsOut[0].coefficientsY[0]*x*x + 
		2*vsOut[0].coefficientsY[1]*x + 
		vsOut[0].coefficientsY[2];
}

void main() {    
	float thickness = vsOut[0].thickness;
	float tilingFactor = vsOut[0].tilingFactor;
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p2 = gl_in[1].gl_Position;
	
	vec2 normal0 = normalize(vec2(-derivativeY(0.), derivativeX(0.)));
	vec2 normal2 = normalize(vec2(-derivativeY(1.), derivativeX(1.)));


	gsOut.color = vec4(1., 1., 1., 1.);
	gsOut.textureIndex = vsOut[0].textureIndex;
	gsOut.entity = vsOut[0].entity;
	gsOut.tilingFactor = vsOut[0].tilingFactor;

	gsOut.uv = vec2(vsOut[0].uv.x*tilingFactor, 0.);
	gl_Position = viewTransform * (p0 - vec4(normal0*thickness, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[0].uv.x*tilingFactor, 1.);
	gl_Position = viewTransform * (p0 + vec4(normal0*thickness, 0., 0.));
	EmitVertex();

	float h = 1/float(segments);
	float hUV = vsOut[1].uv.x - vsOut[0].uv.x;
	float uvX;
	for(int i=1; i<segments; i++){
		float v = h*i;
		uvX = vsOut[0].uv.x + hUV*float(i)/segments;
		vec4 p = vec4(evaluateX(v), evaluateY(v), p0.z, 1.);
		vec2 normal = normalize(vec2(-derivativeY(v), derivativeX(v)));
		gsOut.uv = vec2(uvX*tilingFactor, 0.);
		gl_Position = viewTransform * (p - vec4(normal*thickness, 0., 0.));
		EmitVertex();
		gsOut.uv = vec2(uvX*tilingFactor, 1.);
		gl_Position = viewTransform * (p + vec4(normal*thickness, 0., 0.));
		EmitVertex();

	}
	gsOut.uv = vec2(vsOut[1].uv.x*tilingFactor, 0.);
	gl_Position = viewTransform * (p2 - vec4(normal2*thickness, 0., 0.));
	EmitVertex();
	gsOut.uv = vec2(vsOut[1].uv.x*tilingFactor, 1.);
	gl_Position = viewTransform * (p2 + vec4(normal2*thickness, 0., 0.));
	EmitVertex();


    EndPrimitive();
}  
