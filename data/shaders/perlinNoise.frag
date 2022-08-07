#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float frequency;
uniform float octaves;
uniform float persistence;

const float maxOctaves = 8;

layout(location = 0) out vec4 fragColor;


//seq 0 511 | shuf | sed 's/^\|$//g'|paste -sd, -
#define PERMUTATION_SIZE 512
const float p[PERMUTATION_SIZE] = {
493,305,377,302,80,26,346,287,270,46,508,345,436,266,76,443,75,230,331,474,269,87,254,283,131,318,44,276,143,172,337,140,163,27,64,312,368,4,132,324,169,467,148,403,349,257,445,60,130,262,325,286,298,333,38,74,68,210,223,65,502,397,415,54,355,208,50,81,73,465,272,62,197,200,173,84,280,128,85,56,160,61,353,226,53,122,12,292,393,238,232,323,407,13,412,158,382,429,96,399,466,364,109,220,20,327,342,233,78,311,329,211,114,462,341,357,496,277,306,367,221,6,378,159,165,395,490,167,396,183,268,428,97,180,295,290,332,475,384,507,313,201,260,63,192,106,449,274,178,347,77,315,139,304,252,446,16,400,48,194,79,485,471,288,42,22,390,411,350,133,359,371,362,236,162,435,510,227,405,423,434,426,344,370,500,219,119,41,265,310,174,121,480,494,414,203,202,242,177,145,509,373,100,243,86,9,135,224,431,319,209,256,452,67,303,375,129,376,126,213,506,72,107,454,391,111,123,291,225,321,222,456,47,433,34,112,235,1,285,484,161,317,212,88,43,309,326,154,413,28,402,190,156,289,136,155,251,447,120,307,278,360,284,151,450,297,501,432,358,281,259,372,152,185,234,314,416,328,125,244,215,271,261,468,45,408,83,504,216,89,457,444,492,175,196,440,246,267,425,250,438,460,482,58,453,7,448,40,110,421,115,293,199,23,15,388,488,495,498,503,330,380,461,193,253,481,437,31,458,418,387,69,205,420,343,489,427,147,35,94,348,218,439,263,181,127,98,294,11,157,101,249,374,137,392,124,138,29,170,383,301,351,18,30,116,486,198,59,300,308,24,95,113,33,279,8,14,406,477,476,82,455,417,168,39,497,150,118,470,36,141,473,37,10,352,299,214,21,282,404,296,479,231,478,149,245,186,334,389,316,207,93,204,182,229,171,369,320,275,164,51,25,49,5,55,273,511,394,71,104,322,336,491,339,191,366,398,335,379,248,247,483,92,166,32,195,410,206,153,464,66,176,52,469,255,19,228,90,217,57,419,99,188,451,361,179,499,264,441,103,401,459,17,91,430,463,142,338,354,146,424,3,0,108,340,240,239,365,472,102,184,422,70,241,381,144,505,258,134,363,105,356,117,189,409,442,487,187,386,2,237,385
};

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
}

float lerp(float a, float b, float x) {
    return a + x * (b - a);
}

int inc(int num) {
    return num+1;
}

float grad(int hash, float x, float y, float z) {
    switch(hash & 0xF) {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0; // never happens
    }
}

float perlin(vec3 pos) {
    int xi = int(pos.x) & 255;                              // Calculate the "unit cube" that the point asked will be located in
    int yi = int(pos.y) & 255;                              // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = int(pos.z) & 255;                              // plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
    float xf = pos.x-int(pos.x);
    float yf = pos.y-int(pos.y);
    float zf = pos.z-int(pos.z);

    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = int(p[int(p[int(p[    xi ])+    yi ])+    zi ]);
    aba = int(p[int(p[int(p[    xi ])+inc(yi)])+    zi ]);
    aab = int(p[int(p[int(p[    xi ])+    yi ])+inc(zi)]);
    abb = int(p[int(p[int(p[    xi ])+inc(yi)])+inc(zi)]);
    baa = int(p[int(p[int(p[inc(xi)])+    yi ])+    zi ]);
    bba = int(p[int(p[int(p[inc(xi)])+inc(yi)])+    zi ]);
    bab = int(p[int(p[int(p[inc(xi)])+    yi ])+inc(zi)]);
    bbb = int(p[int(p[int(p[inc(xi)])+inc(yi)])+inc(zi)]);


    float x1, x2, y1, y2;
    x1 = lerp(    grad (aaa, xf  , yf  , zf),           // The gradient function calculates the dot product between a pseudorandom
                grad (baa, xf-1, yf  , zf),             // gradient vector and the vector from the input coordinate to the 8
                u);                                     // surrounding points in its unit cube.
    x2 = lerp(    grad (aba, xf  , yf-1, zf),           // This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
                grad (bba, xf-1, yf-1, zf),             // values we made earlier.
                  u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(    grad (aab, xf  , yf  , zf-1),
                grad (bab, xf-1, yf  , zf-1),
                u);
    x2 = lerp(    grad (abb, xf  , yf-1, zf-1),
                  grad (bbb, xf-1, yf-1, zf-1),
                  u);
    y2 = lerp (x1, x2, v);
    
    return (lerp (y1, y2, w)+1)/2; 
}

float perlin3D(vec3 pos){
	float amplitude = 0.5;
	float value = 0.;
	float f = frequency;
	for(int i=0; i<maxOctaves; i++){
		if(i > octaves) continue;
		value += amplitude*perlin(pos*f);
		amplitude *= persistence;
		f *= 2;
	}
	return value;
}

void main() {
	float value = perlin3D(vec3(uv.x, uv.y, 0.));
	fragColor = vec4(value, value, value, 1.0);
}
