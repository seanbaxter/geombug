#include "app.hxx"
#include <vector>

#if 0
[[spirv::vert]]
void vert_main() {
  // Vertex shader pass-through. Use the geometry shader to amply points into
  // cubes, then project vertices into clip space.
}

[[spirv::geom(points, triangle_strip, 3)]]
void geom_main() {
#ifdef A_BUG
  vec3 vertices[3] {
    { .5, 0, 0 }, { 0, .5, 0 }, { -.5, 0, 0 }
  };
  
  for(int i = 0; i < 3; ++i) {
    // Create a new vertex and project.
    glgeom_Output.Position = vec4(vertices[i], 1);
    
    // Emit the vertex.
    glgeom_EmitVertex(); 
  }

#else // A_BUG

  vec4 vertices[3] {
    { .5, 0, 0, 1 }, { 0, .5, 0, 1 }, { -.5, 0, 0, 1 }
  };

  for(int i = 0; i < 3; ++i) {
    // Create a new vertex and project.
    glgeom_Output.Position = vertices[i];
    
    // Emit the vertex.
    glgeom_EmitVertex(); 
  }

#endif // A_BUG

  // Finish the triangle strip primitive.
  glgeom_EndPrimitive();
}

[[spirv::frag]]
void frag_main() {
  shader_out<0, vec4> = vec4(.6, .2, .2, 1);
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Host code.

struct myapp_t : app_t {
  myapp_t();
  void display() override;

  GLuint program;
  GLuint vao;
};

void load_shader(GLuint shader, const char* module, const char* entry_point) {
  FILE* f = fopen(module, "r");
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);

  std::vector<char> vec;
  vec.resize(len);
  fread(vec.data(), 1, len, f);
  fclose(f);

  glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, 
    vec.data(), len);
  glSpecializeShader(shader, entry_point, 0, nullptr, nullptr);
}

myapp_t::myapp_t() : app_t("Geometry shader demo") {
  // Create a VAO.
  glCreateVertexArrays(1, &vao);

  // Load and compile the shaders.
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

#ifdef BAD
  load_shader(vs, "bad.spv", "_Z9vert_mainv");
  load_shader(gs, "bad.spv", "_Z9geom_mainv");
  load_shader(fs, "bad.spv", "_Z9frag_mainv");
#else
  load_shader(vs, "good.spv", "_Z9vert_mainv");
  load_shader(gs, "good.spv", "_Z9geom_mainv");
  load_shader(fs, "good.spv", "_Z9frag_mainv");
#endif

  // Create the shader program.
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, gs);
  glAttachShader(program, fs);
  glLinkProgram(program);
}

void myapp_t::display() {
  const float bg[4] { .2, .3, .6, 0 };
  glClearBufferfv(GL_COLOR, 0, bg);
  glClear(GL_DEPTH_BUFFER_BIT);

  // Setup the device.
  glUseProgram(program);

  // Render all particles.
  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, 1);

  glBindVertexArray(0);
  glUseProgram(0);
}

int main() {
  glfwInit();
  gl3wInit();
  
  myapp_t myapp;
  myapp.loop();

  return 0;
}