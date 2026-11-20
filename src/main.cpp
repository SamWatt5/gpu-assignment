// Author: Sam Watts
// Main file for CS51012 assignment, adapted from code given in the labs
// A scene of a UFO abducting a cow on a field
// For part 2 i will add a ufo model into the cockpit, change the cow model, add
// a star background, and give the moon a texture will also add some decoration
// around the scene, such as a farmhouse, tractor, fences Will also try to give
// the scene some fog/rain so that the beam path can be seen (this might be out
// of scope!)

/* Link to static libraries, could define these as linker inputs in the project
settings instead if you prefer */
#include "tools/example_objects/quad_tex.h"
#include <map>
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

// We'll use the STD stack class to make our stack or matrices
#include <stack>

/* Include GLM core and matrix extensions*/
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "tools/example_objects/quad_tex.h"
#include "tools/example_objects/sphere.h"

#include "cow.h"
#include "ufo.h"

GLuint program; /* Identifier for the shader program */
GLuint vao;     /* Vertex array (Container) object. This is the index of the VAO
                   that will be the container for our buffer objects */

GLuint colourmode; /* Index of a uniform to switch the colour mode in the vertex
                      shader I've included this to show you how to pass in an
                      unsigned integer into your vertex shader. */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLfloat camera_facing_x, camera_facing_y, camera_facing_z;
GLfloat camera_x, camera_y, camera_z;

GLfloat light_pos_x, light_pos_y, light_pos_z;

GLuint drawmode;          // Defines drawing mode of sphere as points, lines or filled
                          // polygons
GLuint numlats, numlongs; // Define the resolution of the sphere object

/* Uniforms*/
GLuint modelID, viewID, projectionID, emissiveID;
GLuint colourmodeID;
GLuint lightpositionID;
GLuint beamlightposID;
GLuint beamlightdirID;
GLuint beamcolourID;

vec3 beam_light_dir;
vec3 beam_colour;

GLfloat aspect_ratio; /* Aspect ratio of the window defined in the reshape
                         callback*/
GLuint numspherevertices;

/* Global instances of our objects */
Sphere moon;
Ufo ufo;
Cow cow;
Quad ground(false);

GLfloat ufoSpinSpeed;
GLfloat ufo_spin;
GLfloat cow_spin;
GLfloat cow_spin_speed;

std::map<int, bool> keys;

using namespace std;
using namespace glm;

// store dict of keys currently being pressed/held - this means you can hold
// multiple buttons at once
void do_key_checks()
{
  if (keys['A'])
    x -= 0.02f;
  if (keys['D'])
    x += 0.02f;
  if (keys['S'])
    z += 0.02f;
  if (keys['W'])
    z -= 0.02f;
  if (keys['1'])
    ufo.setRotationSpeed(0.f);
  if (keys['2'])
    ufo.setRotationSpeed(1.f);
  if (keys['3'])
    ufo.setRotationSpeed(2.f);
  if (keys['4'])
    ufo.setRotationSpeed(4.f);
  if (keys['5'])
    ufo.setRotationSpeed(6.f);

  if (keys[GLFW_KEY_LEFT_CONTROL] or keys[GLFW_KEY_RIGHT_SHIFT])
    y -= 0.005f;
  if (keys[GLFW_KEY_LEFT_SHIFT])
    y += 0.005f;
  if (keys[GLFW_KEY_UP])
    camera_y += 0.05f;
  if (keys[GLFW_KEY_DOWN])
    camera_y -= 0.05f;
  if (keys[GLFW_KEY_LEFT])
    camera_x -= 0.05f;
  if (keys[GLFW_KEY_RIGHT])
    camera_x += 0.05f;
  if (keys[GLFW_KEY_PAGE_UP])
    camera_z += 0.05f;
  if (keys[GLFW_KEY_PAGE_DOWN])
    camera_z -= 0.05f;
}

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw)
{
  /* Set the object transformation controls to their initial values */
  x = 0;
  y = 0.5f;
  z = 0;
  angle_x = angle_y = angle_z = 0;
  angle_inc_x = angle_inc_y = angle_inc_z = 0;
  model_scale = 1.f;
  aspect_ratio = 1024.f / 768.f; // Initial aspect ratio from window size
                                 // (variables would be better!)
  colourmode = 1;                // 0=use colour attrib buffer, 1=colour defined in shaders
  numlats = 80;                  // Number of latitudes in our sphere
  numlongs = 80;                 // Number of longitudes in our sphere
  camera_facing_x = 0;
  camera_facing_y = 0;
  camera_facing_z = 0;
  camera_x = 0;
  camera_y = 0;
  camera_z = 5;
  light_pos_x = 1;
  light_pos_y = 1;
  light_pos_z = -1;
  ufo_spin = 0;
  cow_spin = 0;
  cow_spin_speed = 0.5f;
  ufoSpinSpeed = ufo.rotationSpeed;

  // Generate index (name) for one vertex array object
  glGenVertexArrays(1, &vao);

  // Create the vertex array object and make it current
  glBindVertexArray(vao);

  /* Load and build the vertex and fragment shaders */
  try
  {
    program = glw->LoadShader("phong.vert", "phong.frag");
  }
  catch (exception &e)
  {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(0);
  }

  /* Define uniforms to send to vertex shader */
  modelID = glGetUniformLocation(program, "model");
  colourmodeID = glGetUniformLocation(program, "colourmode");
  viewID = glGetUniformLocation(program, "view");
  projectionID = glGetUniformLocation(program, "projection");
  lightpositionID = glGetUniformLocation(program, "light_position");
  emissiveID = glGetUniformLocation(program, "emissive");
  beamlightposID = glGetUniformLocation(program, "beam_light_position");
  beamlightdirID = glGetUniformLocation(program, "beam_light_direction");
  beamcolourID = glGetUniformLocation(program, "beam_colour");

  beam_colour = vec3(0.3f, 1.0f,
                     0.5f); // change this to whatever colour you want the beam

  ground.makeQuad();
  ufo.makeUfo(numlats, numlongs, 8);
  cow.makeCow(numlats, numlongs);
  moon.makeSphere(numlats, numlongs);
}

/* Called to update the display. Note that this function is called in the event
loop in the wrapper class because we registered display as a callback function
*/
void display()
{
  do_key_checks();
  ufoSpinSpeed = ufo.rotationSpeed;

  /* Define the background colour */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  /* Clear the colour and frame buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Enable depth test  */
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Make the compiled shader program current */
  glUseProgram(program);

  // Projection matrix : 30° Field of View, 4:3 ratio, display range : 0.1 unit
  // <-> 100 units
  mat4 projection = perspective(radians(30.0f), aspect_ratio, 0.1f, 100.0f);

  vec3 light_pos = vec3(light_pos_x, light_pos_y, light_pos_z);
  vec3 beam_light_pos = vec3(x, y, z);
  vec3 beam_light_dir = vec3(0, -1, 0);

  // i got this maths from https://learnopengl.com/Getting-started/Camera but
  // adapted it to the project
  float camX = sin(camera_x) * camera_z;
  float camZ = cos(camera_x) * camera_z;
  // Camera matrix
  mat4 view =
      lookAt(vec3(camX, camera_y,
                  camZ), // Camera is at (camX, camY, camZ), in World Space
             vec3(camera_facing_x, camera_facing_y,
                  camera_facing_z), // and looks at the origin
             vec3(0, 1, 0)          // Head is up (set to 0,-1,0 to look upside-down)
      );

  // Send our projection and view uniforms to the currently bound shader
  // I do that here because they are the same for all objects
  glUniform1ui(colourmodeID, colourmode);
  glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
  glUniform3fv(lightpositionID, 1, &light_pos[0]);
  glUniform3fv(beamlightposID, 1, &beam_light_pos[0]);
  glUniform3fv(beamlightdirID, 1, &beam_light_dir[0]);
  glUniform3fv(beamcolourID, 1, &beam_colour[0]);

  // Define our model transformation in a stack and
  // push the identity matrix onto the stack
  stack<mat4> model;
  model.push(mat4(1.0f));

  // Define our transformations that apply to all our objects
  // by modifying the matrix at the top of the stack
  model.top() =
      scale(model.top(), vec3(model_scale, model_scale,
                              model_scale)); // scale equally in all axis
  model.top() =
      rotate(model.top(), -radians(angle_x),
             vec3(1, 0, 0)); // rotating in clockwise direction around x-axis
  model.top() =
      rotate(model.top(), -radians(angle_y),
             vec3(0, 1, 0)); // rotating in clockwise direction around y-axis
  model.top() =
      rotate(model.top(), -radians(angle_z),
             vec3(0, 0, 1)); // rotating in clockwise direction around z-axis

  // Push the current value of the top of the matrix onto the stack
  // I like to add a code block here to emphasise  the section between push and
  // pop This block of code draws the cube
  model.push(model.top());
  {
    // Define the model transformations for the cube
    model.top() = translate(model.top(), vec3(x, y, z));
    model.top() = scale(model.top(), vec3(model_scale / 4.f, model_scale / 4.f,
                                          model_scale / 4.f));
    model.top() = rotate(model.top(), -radians(ufo_spin), vec3(0, 1, 0));

    // Send the model uniform to the currently bound shader,
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));

    /* Draw our cube*/
    ufo.drawUfo(model, modelID, emissiveID, drawmode);
  }
  model.pop();

  // Draw cow in scene, in the beam of the ufo, rotating in all axis by
  // cow_spin_speed
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(x, y - 0.75, z));
    model.top() = scale(model.top(), vec3(model_scale / 5.f, model_scale / 5.f,
                                          model_scale / 5.f));
    model.top() = rotate(model.top(), -radians(cow_spin), vec3(1, 1, 1));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
    cow.drawCow(model, modelID, drawmode);
  }
  model.pop();

  // Draw moon in scene at the light position - not in the correct position for
  // now (its small and near the rest of scene) will also make this a
  // directional light instead of positional - need to adapt shader
  model.push(model.top());
  {
    model.top() =
        translate(model.top(), vec3(light_pos_x, light_pos_y, light_pos_z));
    model.top() = scale(model.top(), vec3(model_scale / 5.f, model_scale / 5.f,
                                          model_scale / 5.f));
    glUniform3fv(emissiveID, 1, &vec3(1.0f, 1.0f, 1.0f)[0]);
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);

    moon.drawSphere(drawmode);
    glUniform3fv(emissiveID, 1, &vec3(0.f)[0]);
  }
  model.pop();

  // draw ground at bottom of scene (green quad)
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(0, -1, 0));
    model.top() = scale(model.top(), vec3(model_scale * 10, model_scale * 10,
                                          model_scale * 10));
    // Undo the quad's own local z=-0.25 offset (it's built off-centre,
    // meant for a camera-facing panel) BEFORE the scale above amplifies it
    model.top() = translate(model.top(), vec3(0, 0.25f, 0));
    model.top() = rotate(model.top(), -radians(90.f), vec3(1, 0, 0));
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);

    ground.drawQuad(drawmode);
  }
  model.pop();

  glDisableVertexAttribArray(0);
  glUseProgram(0);

  /* Modify our animation variables */
  angle_x += angle_inc_x;
  angle_y += angle_inc_y;
  angle_z += angle_inc_z;
  ufo_spin += ufoSpinSpeed;
  cow_spin += cow_spin_speed;
}

/* Called whenever the window is resized. The new window size is given, in
 * pixels. */
static void reshape(GLFWwindow *window, int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  // Store aspect ratio to use for our perspective projection
  aspect_ratio = float(w) / float(h);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window, int key, int s, int action,
                        int mods)
{
  /* Enable this call if you want to disable key responses to a held down key*/
  // if (action != GLFW_PRESS) return;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  // Found this better way to do key presses storing a dict of keys currently
  // pressed instead of checking each key in this function
  if (action == GLFW_PRESS)
    keys[key] = true;
  else if (action == GLFW_RELEASE)
    keys[key] = false;

  if (key == 'M' && action != GLFW_PRESS)
    colourmode = !colourmode;

  /* Cycle between drawing vertices, mesh and filled polygons */
  if (key == ',' && action != GLFW_PRESS)
  {
    drawmode++;
    if (drawmode > 2)
      drawmode = 0;
  }
}

/* Entry point of program */
int main(int argc, char *argv[])
{
  GLWrapper *glw = new GLWrapper(1024, 768, "Ufo Abducting a cow!!!");
  ;

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    cout << "Failed to initialize GLAD. Exiting." << endl;
    return -1;
  }

  // Register the callback functions
  glw->setRenderer(display);
  glw->setKeyCallback(keyCallback);
  glw->setReshapeCallback(reshape);

  /* Output the OpenGL vendor and version */
  glw->DisplayVersion();

  init(glw);

  glw->eventLoop();

  delete (glw);
  return 0;
}
