// Author: Sam Watts
// Header file for cow object
#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "tools/example_objects/cylinder.h"
#include "tools/example_objects/sphere.h"
#include "wrapper_glfw.h"
#include <glm/glm.hpp>
using namespace glm;
#include <stack>

class Cow {
public:
  Cow();
  ~Cow();

  void makeCow(GLuint numlats, GLuint numlongs);
  void drawCow(std::stack<mat4> &model, GLuint modelID, int drawmode);

private:
  Cylinder body;
  Cylinder leg;
  Sphere head;
};
