#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 viewTransform;

in VsOut {
	float size;
	float rotation;
	vec4 color;
	float textureIndex;
} vsOut[];

out GsOut {
	vec2 uv;
	vec4 color;
	float textureIndex;
} gsOut;


vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

mat4 identityMatrix(){
	return  mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
			     0.0, 1.0, 0.0, 0.0,  // 2. column
                 0.0, 0.0, 1.0, 0.0,  // 3. column
                 0.0, 0.0, 0.0, 1.0); // 4. column
}

mat4 translateMatrix(vec4 desp){
	return  mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
			     0.0, 1.0, 0.0, 0.0,  // 2. column
                 0.0, 0.0, 1.0, 0.0,  // 3. column
                 desp.x, desp.y, desp.z, 1.0); // 4. column
}

mat4 translateMatrix(vec3 desp){
	return  mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
			     0.0, 1.0, 0.0, 0.0,  // 2. column
                 0.0, 0.0, 1.0, 0.0,  // 3. column
                 desp.x, desp.y, desp.z, 1.0); // 4. column
}


mat4 scaleMatrix(vec3 desp){
	return  mat4(desp.x, 0.0, 0.0, 0.0,  // 1. column
			     0.0, desp.y, 0.0, 0.0,  // 2. column
                 0.0, 0.0, desp.z, 0.0,  // 3. column
                 0.0, 0.0, 0.0,   1.0); // 4. column
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {    
	gsOut.color = vsOut[0].color;
	gsOut.textureIndex = vsOut[0].textureIndex;

	mat4 transformation = viewTransform * 
		translateMatrix(gl_in[0].gl_Position) *
		rotationMatrix(vec3(0.0, 0.0, 1.0), vsOut[0].rotation) *
		scaleMatrix(vec3(vsOut[0].size, vsOut[0].size, 1.0));
	
	gl_Position = transformation * vec4(-0.5, -0.5, 0., 1.);
	gsOut.uv = vec2(0., 0.);
	EmitVertex();

	gl_Position = transformation * vec4(0.5, -0.5, 0., 1.);
	gsOut.uv = vec2(1., 0.);
	EmitVertex();

	gl_Position = transformation * vec4(-0.5, 0.5, 0., 1.);
	gsOut.uv = vec2(0., 1.);
	EmitVertex();

	gl_Position = transformation * vec4(0.5, 0.5, 0., 1.);
	gsOut.uv = vec2(1., 1.);
	EmitVertex();

    
    EndPrimitive();
}  
