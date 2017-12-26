#ifndef OPENGL_SHADERS_HPP_INCLUDED
#define OPENGL_SHADERS_HPP_INCLUDED

#include <glm/glm.hpp>

#include "backends/opengl/OpenglBuffer.hpp"
#include "backends/opengl/OpenglSurface.hpp"

namespace Kvant::graphics::opengl {

class OpenglRenderPassClearCmd {
public:
  OpenglRenderPassClearCmd() {}
  void apply(vec4 c = vec4(0.9, 0.9, 0.9, 0.0)) const {
    GL_CHECK(glClearColor(c.x, c.y, c.z, c.w));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                     GL_STENCIL_BUFFER_BIT));
  }
};

template <typename GRAPHICS, typename PLATFORM>
class OpenglPipeline : public blueprints::graphics::Pipeline<GRAPHICS> {
  friend struct OpenglSurface<GRAPHICS>;

public:
  OpenglPipeline(std::string fragment, std::string vertex);
  OpenglPipeline(std::string name);
  virtual ~OpenglPipeline();

  void use(const GraphicsContext<GRAPHICS> &ctx) const;
  void unuse(const GraphicsContext<GRAPHICS> &ctx) const;

  using Sampler = GLint;
  using Uniform = GLint;
  using Attrib = GLint;

  OpenglPipeline(const OpenglPipeline &) = delete;
  OpenglPipeline &operator=(const OpenglPipeline &) = delete;

  Sampler add_sampler(std::string id) const {
    GLint ret =
        GL_CHECK(glGetUniformLocation(this->shader_program, id.c_str()));
    if (ret == -1)
      LOG_WARNING << "Cannot find shader sampler named: " << id;
    return ret;
  }

  Uniform add_uniform(std::string id) const {
    GLint ret =
        GL_CHECK(glGetUniformLocation(this->shader_program, id.c_str()));
    if (ret == -1)
      LOG_WARNING << "Cannot find uniform named: " << id;
    return ret;
  }

  Attrib add_attrib(std::string id) const {
    GLint ret = GL_CHECK(glGetAttribLocation(this->shader_program, id.c_str()));
    if (ret == -1)
      LOG_WARNING << "Cannot find shader attribute named: " << id;
    return ret;
  }

  void bind_sampler(Sampler id, const OpenglColorBuffer<GRAPHICS> *t) const {
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + t->unit));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, t->tex));
    GL_CHECK(glUniform1i(id, t->unit));
  }

  void bind_uniform(Uniform id, glm::mat4 mat) const {
    GL_CHECK(glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat)));
  }

  void bind_uniform(Uniform id, glm::vec4 v) const {
    GL_CHECK(glUniform4fv(id, 1, glm::value_ptr(v)));
  }

  void bind_uniform(Uniform id, glm::vec3 v) const {
    GL_CHECK(glUniform3fv(id, 1, glm::value_ptr(v)));
  }

  void bind_uniform(Uniform id, glm::vec2 v) const {
    GL_CHECK(glUniform2fv(id, 1, glm::value_ptr(v)));
  }

  // TODO: implement more function overloads
  /*void bind_uniform(Uniform id, std::vector<glm::vec4> v) const {
      GL_CHECK(glUniform4fv(id, 1, glm::value_ptr(v)));
  }*/

  void bind_uniform(Uniform id, float v) const { GL_CHECK(glUniform1f(id, v)); }

  void bind_float_attrib(Attrib id, int count, int stride, void *ptr) const {
    GL_CHECK(glVertexAttribPointer(id, count, GL_FLOAT, GL_FALSE,
                                   sizeof(float) * stride, ptr));
    GL_CHECK(glEnableVertexAttribArray(id));
  }

  void bind_instances_data(Attrib id) const {
    for (unsigned int i = 0; i < 4; i++) {
      glEnableVertexAttribArray(id + i);
      glVertexAttribPointer(id + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                            (const GLvoid *)(sizeof(GLfloat) * i * 4));
      glVertexAttribDivisor(id + i, 1);
    }
  }

  void disable_attrib(Attrib id) const { glDisableVertexAttribArray(id); }

protected:
  GLuint shader_program, shader_frag, shader_vert, last_program;
  GraphicsContext<GRAPHICS> adhoc_context() const {
    return GraphicsContext<GRAPHICS>{_graphics};
  }

private:
  mutable GRAPHICS *_graphics;
  void check_shader(GLuint shader) const;
  void check_program(GLuint program) const;
  void load_shader(GLuint *shader, std::string buf, GLint shader_type) const;
};

template <typename GRAPHICS, typename PLATFORM>
OpenglPipeline<GRAPHICS, PLATFORM>::OpenglPipeline(std::string fragment,
                                                   std::string vertex) {
  LOG_DEBUG << "Initializing Opengl Pipeline with fragment: " << fragment
            << ", vertex: " << vertex;
  load_shader(&this->shader_vert, PLATFORM::read_file(vertex),
              GL_VERTEX_SHADER);
  LOG_DEBUG << "Successfully loaded vertex shader";
  load_shader(&this->shader_frag, PLATFORM::read_file(fragment),
              GL_FRAGMENT_SHADER);
  LOG_DEBUG << "Successfully loaded fragment shader";
  this->shader_program = GL_CHECK(glCreateProgram());
  GL_CHECK(glAttachShader(this->shader_program, this->shader_vert));
  GL_CHECK(glAttachShader(this->shader_program, this->shader_frag));
  GL_CHECK(glLinkProgram(this->shader_program));
}

template <typename GRAPHICS, typename PLATFORM>
OpenglPipeline<GRAPHICS, PLATFORM>::OpenglPipeline(std::string base_name)
    : OpenglPipeline(base_name + ".frag", base_name + ".vert") {}

template <typename GRAPHICS, typename PLATFORM>
OpenglPipeline<GRAPHICS, PLATFORM>::~OpenglPipeline() {
  glDetachShader(this->shader_program, this->shader_vert);
  glDetachShader(this->shader_program, this->shader_frag);
  glDeleteProgram(this->shader_program);
  glDeleteShader(this->shader_vert);
  glDeleteShader(this->shader_frag);
}

template <typename GRAPHICS, typename PLATFORM>
void OpenglPipeline<GRAPHICS, PLATFORM>::load_shader(GLuint *shader,
                                                     std::string buf,
                                                     GLint shader_type) const {
  *shader = GL_CHECK(glCreateShader(shader_type));
  const char *cbuf = buf.c_str();
  GL_CHECK(glShaderSource(*shader, 1, &cbuf, NULL));
  GL_CHECK(glCompileShader(*shader));
  check_shader(*shader);
}

template <typename GRAPHICS, typename PLATFORM>
void OpenglPipeline<GRAPHICS, PLATFORM>::check_shader(GLuint shader) const {
  GLint status = 1;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    GLint infolen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolen);
    if (infolen > 1) {
      char *infolog = (char *)malloc(sizeof(char) * infolen);
      glGetShaderInfoLog(shader, infolen, NULL, infolog);
      LOG_ERROR << "Error compiling shader: " << infolog;
      free(infolog);
      exit(1);
    }
  }
}

template <typename GRAPHICS, typename PLATFORM>
void OpenglPipeline<GRAPHICS, PLATFORM>::check_program(GLuint program) const {
  GLint status = 1;
  glGetProgramiv(program, GL_COMPILE_STATUS, &status);
  if (!status) {
    GLint infolen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolen);
    if (infolen > 1) {
      char *infolog = (char *)malloc(sizeof(char) * infolen);
      glGetProgramInfoLog(program, infolen, NULL, infolog);
      LOG_ERROR << "Error compiling shader: " << infolog;
      free(infolog);
      exit(1);
    }
  }
}

template <typename GRAPHICS, typename PLATFORM>
void OpenglPipeline<GRAPHICS, PLATFORM>::use(
    const GraphicsContext<GRAPHICS> &ctx) const {
  GL_CHECK(glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&this->last_program));
  GL_CHECK(glUseProgram(this->shader_program));

  _graphics = ctx.graphics;
}

template <typename GRAPHICS, typename PLATFORM>
void OpenglPipeline<GRAPHICS, PLATFORM>::unuse(
    const GraphicsContext<GRAPHICS> &ctx) const {
  GL_CHECK(glUseProgram(this->last_program));
  _graphics = nullptr;
}

} // namespace Kvant::graphics::opengl

#endif // OPENGL_SHADERS_HPP_INCLUDED
