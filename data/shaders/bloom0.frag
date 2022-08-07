#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float threshold;

layout(location = 0) out vec4 fragColor;

#define KERNEL_WIDTH 129

const float kernel[KERNEL_WIDTH] = {3.47370566e-07, 4.93987834e-07, 6.98604295e-07, 9.82512330e-07,
       1.37415755e-06, 1.91129096e-06, 2.64367974e-06, 3.63649204e-06,
       4.97448584e-06, 6.76714554e-06, 9.15492088e-06, 1.23167303e-05,
       1.64788929e-05, 2.19256456e-05, 2.90113880e-05, 3.81747662e-05,
       4.99546620e-05, 6.50080925e-05, 8.41299404e-05, 1.08274329e-04,
       1.38577326e-04, 1.76380519e-04, 2.23254805e-04, 2.81023588e-04,
       3.51784332e-04, 4.37927251e-04, 5.42149685e-04, 6.67464556e-04,
       8.17201146e-04, 9.94996346e-04, 1.20477449e-03, 1.45071400e-03,
       1.73719913e-03, 2.06875548e-03, 2.44996839e-03, 2.88538360e-03,
       3.37939065e-03, 3.93608979e-03, 4.55914449e-03, 5.25162220e-03,
       6.01582722e-03, 6.85313037e-03, 7.76380096e-03, 8.74684745e-03,
       9.79987354e-03, 1.09189569e-02, 1.20985574e-02, 1.33314623e-02,
       1.46087733e-02, 1.59199415e-02, 1.72528540e-02, 1.85939726e-02,
       1.99285264e-02, 2.12407549e-02, 2.25141972e-02, 2.37320222e-02,
       2.48773884e-02, 2.59338256e-02, 2.68856255e-02, 2.77182282e-02,
       2.84185912e-02, 2.89755298e-02, 2.93800131e-02, 2.96254083e-02,
       2.97076614e-02, 2.96254083e-02, 2.93800131e-02, 2.89755298e-02,
       2.84185912e-02, 2.77182282e-02, 2.68856255e-02, 2.59338256e-02,
       2.48773884e-02, 2.37320222e-02, 2.25141972e-02, 2.12407549e-02,
       1.99285264e-02, 1.85939726e-02, 1.72528540e-02, 1.59199415e-02,
       1.46087733e-02, 1.33314623e-02, 1.20985574e-02, 1.09189569e-02,
       9.79987354e-03, 8.74684745e-03, 7.76380096e-03, 6.85313037e-03,
       6.01582722e-03, 5.25162220e-03, 4.55914449e-03, 3.93608979e-03,
       3.37939065e-03, 2.88538360e-03, 2.44996839e-03, 2.06875548e-03,
       1.73719913e-03, 1.45071400e-03, 1.20477449e-03, 9.94996346e-04,
       8.17201146e-04, 6.67464556e-04, 5.42149685e-04, 4.37927251e-04,
       3.51784332e-04, 2.81023588e-04, 2.23254805e-04, 1.76380519e-04,
       1.38577326e-04, 1.08274329e-04, 8.41299404e-05, 6.50080925e-05,
       4.99546620e-05, 3.81747662e-05, 2.90113880e-05, 2.19256456e-05,
       1.64788929e-05, 1.23167303e-05, 9.15492088e-06, 6.76714554e-06,
       4.97448584e-06, 3.63649204e-06, 2.64367974e-06, 1.91129096e-06,
       1.37415755e-06, 9.82512330e-07, 6.98604295e-07, 4.93987834e-07,
       3.47370566e-07
};

float rgb2Light(vec4 c){
	return dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
	vec4 bloomColor = vec4(0.);
	ivec2 originPos = ivec2(gl_FragCoord.xy);
	for(int x=-KERNEL_WIDTH/2; x<KERNEL_WIDTH/2+1; x++){
		float alpha = kernel[x+(KERNEL_WIDTH/2)];
		vec4 fetchColor = texelFetch(u_Texture, originPos + ivec2(0,x), 0);
		if(rgb2Light(fetchColor) > threshold){
			bloomColor += alpha*fetchColor;
		}
	}

	fragColor = bloomColor;
	fragColor.a = 1.;
}