// Author: Sam Watts
// Header file for ufo object
#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "tools/example_objects/cylinder.h"
#include "tools/example_objects/sphere.h"
#include "wrapper_glfw.h"
#include <glm/glm.hpp>
using namespace glm;
#include <stack>

class Ufo {
public:
  Ufo();
  ~Ufo();

  void makeUfo(GLuint numlats, GLuint numlongs, GLuint numlights,
               vec4 bodycolour = vec4(0.6f, 0.6f, 0.65f, 1.0f),
               vec4 cockpitcolour = vec4(0.3f, 0.8f, 0.9f, 0.4f));
  void drawUfo(std::stack<mat4> &model, GLuint modelID, GLuint emissiveID,
               int drawmode);
  float rotationSpeed;
  void setRotationSpeed(GLfloat newSpinSpeed);

private:
  Sphere body;
  Sphere cockpit;
  Sphere light;
  Sphere beamer;

  GLuint numLights;
};
