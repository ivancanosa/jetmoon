#version 450 core


layout (location = 0) in vec3 position;
layout (location = 1) in float size;
layout (location = 2) in float rotation;
layout (location = 3) in vec4 colorIn;
layout (location = 4) in float iTextureIndex;

out VsOut {
	float size;
	float rotation;
	vec4 color;
	float textureIndex;
} vsOut;


uniform mat4 viewTransform;


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
/*	mat4 transformation = 
		translateMatrix(position) *
		rotationMatrix(vec3(0.0, 0.0, 1.0), rotation) *
		scaleMatrix(vec3(size, size, 1.0));



    gl_Position =  viewTransform * transformation * vec4(iPos, 1.0);
	uv = iUV;
	color = colorIn;
	textureIndex = iTextureIndex;*/
    gl_Position = vec4(position, 1.0);
	vsOut.color = colorIn;
	vsOut.size = size;
	vsOut.rotation = rotation;
	vsOut.textureIndex = iTextureIndex;
}
