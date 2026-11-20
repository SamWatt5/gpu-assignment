// Author: Sam Watts
// Source file for UFO object
#include "ufo.h"

Ufo::Ufo() : numLights(0), rotationSpeed(2.f) {}

Ufo::~Ufo() {}

// Make the ufo
void Ufo::makeUfo(GLuint numlats, GLuint numlongs, GLuint numlights,
                  vec4 bodycolour, vec4 cockpitcolour) {
  numLights = numlights;
  body.makeSphere(numlats, numlongs, bodycolour);
  cockpit.makeSphere(numlats, numlongs, cockpitcolour);
  light.makeSphere(numlats, numlongs, vec4(1.f, 1.f, 0.6f, 1.0f));
  beamer.makeSphere(numlats, numlongs, vec4(1.f, 1.f, 0.6f, 0.7f));
}

// draw ufo to given model, emissiveID is also passed in for the lights on the
// body
void Ufo::drawUfo(std::stack<mat4> &model, GLuint modelID, GLuint emissiveID,
                  int drawmode) {
  // Body: squashed sphere
  model.push(model.top());
  {
    model.top() = scale(model.top(), vec3(1.2f, 0.3f, 1.2f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    glUniform3fv(emissiveID, 1, &vec3(0.f)[0]);
    body.drawSphere(drawmode);
  }
  model.pop();

  // Lights: one cylinder drawn multiple times around rim of body
  for (GLuint i = 0; i < numLights; i++) {
    float angle = radians(360.f * i / numLights);

    model.push(model.top());
    {
      model.top() = rotate(model.top(), angle, vec3(0, 1, 0));
      model.top() = translate(model.top(), vec3(1.1f, -0.1f, 0.f));
      model.top() = scale(model.top(), vec3(0.08f, 0.08f, 0.08f));
      glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
      glUniform3fv(emissiveID, 1, &vec3(0.3f, 1.0f, 0.5f)[0]);
      light.drawSphere(drawmode);
      glUniform3fv(emissiveID, 1, &vec3(0.f)[0]);
    }
    model.pop();
  }

  // beam light thing: squished sphere under body of ufo
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.f, -0.25f, 0.f));
    model.top() = scale(model.top(), vec3(0.5f, 0.1f, 0.5f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    glUniform3fv(emissiveID, 1, &vec3(0.3f, 1.0f, 0.5f)[0]);
    beamer.drawSphere(drawmode);
    glUniform3fv(emissiveID, 1, &vec3(0.f)[0]);
  }
  model.pop();

  // Cockpit: smaller sphere on top of body
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0.f, 0.25f, 0.f));
    model.top() = scale(model.top(), vec3(0.75f, 0.5f, 0.75f));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    glUniform3fv(emissiveID, 1, &vec3(0.f)[0]);
    glDepthMask(GL_FALSE);
    cockpit.drawSphere(drawmode);
    glDepthMask(GL_TRUE);
  }
  model.pop();
}

void Ufo::setRotationSpeed(GLfloat newSpinSpeed) {
  rotationSpeed = newSpinSpeed;
}
