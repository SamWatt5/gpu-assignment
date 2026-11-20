// Author: Sam Watts
// Source file for cow object (for part 2 i will make this an actual cow model)
#include "cow.h"

Cow::Cow() {}

Cow::~Cow() {}

// Make cow - moo
void Cow::makeCow(GLuint numlats, GLuint numlongs) {
  body.makeCylinder();
  head.makeSphere(numlats, numlongs, vec4(1.f));
  leg.makeCylinder();
}

// Draw Cow body on given model
void Cow::drawCow(std::stack<mat4> &model, GLuint modelID, int drawmode) {
  // Body: Cylinder on its side
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.f, 0.5f, 0.f));
    model.top() = rotate(model.top(), radians(90.f), vec3(1.f, 0.f, 0.f));
    model.top() = scale(model.top(), vec3(0.3f, 1.3f, 0.3f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    body.drawCylinder(drawmode);
  }
  model.pop();

  // Legs: four small cylinders each added manually
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.2f, -0.05f, 0.5f));
    model.top() = scale(model.top(), vec3(0.07f, 0.5f, 0.07f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    leg.drawCylinder(drawmode);
  }
  model.pop();

  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(-0.2f, -0.05f, 0.5f));
    model.top() = scale(model.top(), vec3(0.07f, 0.5f, 0.07f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    leg.drawCylinder(drawmode);
  }
  model.pop();

  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(-0.2f, -0.05f, -0.5f));
    model.top() = scale(model.top(), vec3(0.07f, 0.5f, 0.07f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    leg.drawCylinder(drawmode);
  }
  model.pop();

  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.2f, -0.05f, -0.5f));
    model.top() = scale(model.top(), vec3(0.07f, 0.5f, 0.07f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    leg.drawCylinder(drawmode);
  }
  model.pop();

  // Head: round sphere at the front end of the body
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.f, 0.5f, 0.85f));
    model.top() = scale(model.top(), vec3(0.25f, 0.25f, 0.25f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    head.drawSphere(drawmode);
  }
  model.pop();
}
