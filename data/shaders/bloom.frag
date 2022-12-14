#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float intensity;
uniform float threshold;

layout(location = 0) out vec4 fragColor;

#define KERNEL_SIZE  121
#define KERNEL_WIDTH 11 

const float kernel[KERNEL_SIZE] = {
0.00709955, 0.00742633, 0.00769085, 0.00788554, 0.00800472,
        0.00804484, 0.00800472, 0.00788554, 0.00769085, 0.00742633,
        0.00709955,
       0.00742633, 0.00776814, 0.00804484, 0.0082485 , 0.00837316,
        0.00841513, 0.00837316, 0.0082485 , 0.00804484, 0.00776814,
        0.00742633,
       0.00769085, 0.00804484, 0.0083314 , 0.00854231, 0.00867141,
        0.00871487, 0.00867141, 0.00854231, 0.0083314 , 0.00804484,
        0.00769085,
       0.00788554, 0.0082485 , 0.00854231, 0.00875856, 0.00889093,
        0.00893549, 0.00889093, 0.00875856, 0.00854231, 0.0082485 ,
        0.00788554,
       0.00800472, 0.00837316, 0.00867141, 0.00889093, 0.0090253 ,
        0.00907054, 0.0090253 , 0.00889093, 0.00867141, 0.00837316,
        0.00800472,
       0.00804484, 0.00841513, 0.00871487, 0.00893549, 0.00907054,
        0.009116  , 0.00907054, 0.00893549, 0.00871487, 0.00841513,
        0.00804484,
       0.00800472, 0.00837316, 0.00867141, 0.00889093, 0.0090253 ,
        0.00907054, 0.0090253 , 0.00889093, 0.00867141, 0.00837316,
        0.00800472,
       0.00788554, 0.0082485 , 0.00854231, 0.00875856, 0.00889093,
        0.00893549, 0.00889093, 0.00875856, 0.00854231, 0.0082485 ,
        0.00788554,
       0.00769085, 0.00804484, 0.0083314 , 0.00854231, 0.00867141,
        0.00871487, 0.00867141, 0.00854231, 0.0083314 , 0.00804484,
        0.00769085,
       0.00742633, 0.00776814, 0.00804484, 0.0082485 , 0.00837316,
        0.00841513, 0.00837316, 0.0082485 , 0.00804484, 0.00776814,
        0.00742633,
       0.00709955, 0.00742633, 0.00769085, 0.00788554, 0.00800472,
        0.00804484, 0.00800472, 0.00788554, 0.00769085, 0.00742633,
        0.00709955
};

float rgb2Light(vec4 c){
	return dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
	vec4 origin = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
	vec4 bloomColor = vec4(0.);
	ivec2 originPos = ivec2(gl_FragCoord.xy);
	for(int x=-KERNEL_WIDTH/2; x<KERNEL_WIDTH/2+1; x++){
		for(int y=-KERNEL_WIDTH/2; y<KERNEL_WIDTH/2+1; y++){
			float alpha = kernel[x+(KERNEL_WIDTH/2) + (y+(KERNEL_WIDTH/2))*KERNEL_WIDTH];
			vec4 fetchColor = texelFetch(u_Texture, originPos + ivec2(x,y), 0);
			if(rgb2Light(fetchColor) > threshold){
				bloomColor += alpha*fetchColor;
			}
		}
	}

	fragColor = vec4(origin.rgb + intensity*bloomColor.rgb, 1.);
}
