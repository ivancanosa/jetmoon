#version 450 core
in vec2 uv;

uniform sampler2D u_Texture;

uniform float frequency;

layout(location = 0) out vec4 fragColor;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float cellularNoiseF1(ivec2 resolution, vec2 pos, float scale){
    vec2 st = pos;
    st.x *= resolution.x/resolution.y;

    // Scale
    st *= scale;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.;  // minimum distance
	vec2 m_point = vec2(0., 0.);

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
			if(m_dist > dist){
                m_dist = dist;
                m_point = point;
			}
        }
    }
	return m_dist;
}

float cellularNoiseF2(ivec2 resolution, vec2 pos, float scale){
    vec2 st = pos;
    st.x *= resolution.x/resolution.y;

    // Scale
    st *= scale;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist0 = 10.;  // minimum distance
    float m_dist1 = 11.;  // minimum distance
	vec2 m_point = vec2(0., 0.);

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
			if(dist < m_dist0){
				m_dist1 = m_dist0;
				m_dist0 = dist;
			}else if(dist < m_dist1){
				m_dist1 = dist;
			}
        }
    }
	return m_dist1;
}

void main() {
	ivec2 size = textureSize(u_Texture, 0);

    // Draw the min distance (distance field)
	float value = cellularNoiseF1(size, uv, frequency);
    vec3 color = vec3(value);


    fragColor = vec4(color,1.0);

}
