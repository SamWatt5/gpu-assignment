// Author: Sam Watts
// Vertex shader for CS51012 assignment
#version 420

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// Uniform variables are passed in from the application
// model matrix: local space => world space
// view matrix: world space => view space (what the camera sees)
// projection matrix: introduces perspective, FOV, clipping planes etc.
uniform mat4 model, view, projection;

uniform uint colourmode;

out vec3 N;
out vec4 P;
out vec4 diffuse_colour;

void main() {
  // this is just position, but homogenised to a vec4
  // could also define position as vec4 in line 25 (as of writing)
  vec4 position_h = vec4(position, 1.0);

  if (colourmode == 1)
    diffuse_colour = colour;
  else
    diffuse_colour = vec4(0.0, 1.0, 0, 1.0);

  // Model view matrix
  // This just combines model and view matrices
  // into a single step (local space => view space)
  mat4 mv_matrix = view * model;

  // create a normal matrix
  // This is a matrix that you multiply with the normal to put it into view
  // space
  // transpose of inverse is used as it works for non-uniform scaling
  mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));

  N = n_matrix * normal;
  P = mv_matrix * position_h; // Calculate vertex position in eye space

  gl_Position = projection * mv_matrix * position_h;
}
