#pragma once
#include <gl3w/GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <cfloat>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////

class app_t {
public:
  app_t(const char* name, int width = 800, int height = 600);
  void loop();
  virtual void display() { }

  virtual void framebuffer_callback(int width, int height);

  virtual void debug_callback(GLenum source, GLenum type, GLuint id, 
    GLenum severity, GLsizei length, const GLchar* message);

protected:
  GLFWwindow* window = nullptr;

private:
  static void _framebuffer_callback(GLFWwindow* window, int width, int height);

  static void _debug_callback(GLenum source, GLenum type, GLuint id, 
    GLenum severity, GLsizei length, const GLchar* message, 
    const void* user_param);

  void register_callbacks();
};

app_t::app_t(const char* name, int width, int height) {
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_SAMPLES, 4); // HQ 4x multisample.
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  window = glfwCreateWindow(width, height, name, nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  register_callbacks();

  gl3wInit();
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(_debug_callback, this);

  glfwGetWindowSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

void app_t::register_callbacks() {
  glfwSetFramebufferSizeCallback(window, _framebuffer_callback);
}

void app_t::loop() {
  while(!glfwWindowShouldClose(window)) {
    display();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void app_t::framebuffer_callback(int width, int height) {
  glViewport(0, 0, width, height);
}

void app_t::debug_callback(GLenum source, GLenum type, GLuint id, 
  GLenum severity, GLsizei length, const GLchar* message) { 

  printf("OpenGL: %s\n", message);
}

void app_t::_framebuffer_callback(GLFWwindow* window, int width, int height) {
  app_t* app = static_cast<app_t*>(glfwGetWindowUserPointer(window));
  app->framebuffer_callback(width, height);
}

void app_t::_debug_callback(GLenum source, GLenum type, GLuint id, 
  GLenum severity, GLsizei length, const GLchar* message, 
  const void* user_param) {

  app_t* app = (app_t*)user_param;
  app->debug_callback(source, type, id, severity, length, message);
}

////////////////////////////////////////////////////////////////////////////////