#version 450 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba16f, binding = 0) uniform image2D out_tex;

void main() {
  // base pixel colour for image
  vec4 pixel = vec4(0.5, 0.0, 0.0, 1.0);
  // get index in global work group i.e x,y position
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
  
  pixel += imageLoad( out_tex, pixel_coords );

  //
  // interesting stuff happens here later
  //
  
  // output to a specific pixel in the image
  imageStore(out_tex, pixel_coords, vec4(0., 0., 0., 1.));
}
