#ifndef OPENGL_BUFFER_HPP_INCLUDED
#define OPENGL_BUFFER_HPP_INCLUDED

#include <vector>

#include "core/Context.hpp"
#include "utils/Checks.hpp"

namespace Kvant::graphics::opengl {

template <typename B> struct OpenglColorBuffer {
  static int counter;
  GLuint tex;
  GLuint unit;
  unsigned int width;
  unsigned int height;

  OpenglColorBuffer(const GraphicsContext<B> &ctx);
  virtual ~OpenglColorBuffer();

  virtual void create(unsigned int w, unsigned int h) {}
};

template <typename B> int OpenglColorBuffer<B>::counter = 0;

template <typename B>
OpenglColorBuffer<B>::OpenglColorBuffer(const GraphicsContext<B> &ctx) {
  this->unit = OpenglColorBuffer<B>::counter++;
  GL_CHECK(glGenTextures(1, &this->tex));
}

template <typename B> OpenglColorBuffer<B>::~OpenglColorBuffer() {
  GL_CHECK(glDeleteTextures(1, &tex));
}

template <typename GRAPHICS>
struct OpenglRgbaBuffer : OpenglColorBuffer<GRAPHICS> {
  OpenglRgbaBuffer(const GraphicsContext<GRAPHICS> &ctx)
      : OpenglColorBuffer<GRAPHICS>(ctx) {}
  virtual ~OpenglRgbaBuffer() {}

  void create(unsigned int w, unsigned int h) override {
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, this->tex));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, NULL));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    this->width = w;
    this->height = h;
  }
};

template <typename GRAPHICS> class OpenglBaseBuffer {
public:
  OpenglBaseBuffer() {}
  virtual ~OpenglBaseBuffer() {}

  unsigned int width() const { return _w; }
  unsigned int height() const { return _h; }

  void bind() const { GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _buffer)); }
  void bind_for_read() const {
    GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, _buffer));
  }
  void bind_for_draw() const {
    GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _buffer));
  }

  void copy_depth_from(const OpenglBaseBuffer<GRAPHICS> &b) const {
    b.bind_for_read();
    bind_for_draw();
    glBlitFramebuffer(0, 0, b.width(), b.height(), 0, 0, b.width(), b.height(),
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    bind();
  }

  virtual void resize(unsigned int w, unsigned int h) {
    _w = w;
    _h = h;
  }

protected:
  GLuint _buffer = 0;
  unsigned int _w, _h;
};

template <typename B> struct OpenglTexture : OpenglColorBuffer<B> {
  OpenglTexture(const GraphicsContext<B> &ctx, std::string filename);
  void create_texture(const char *filename);
};

template <typename B>
OpenglTexture<B>::OpenglTexture(const GraphicsContext<B> &ctx,
                                std::string filename)
    : OpenglColorBuffer<B>(ctx) {
  create_texture(filename.c_str());
}

template <typename B>
void OpenglTexture<B>::create_texture(const char *filename) {
  unsigned char *img;
  unsigned int ww, hh;
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, this->tex));
  if (load_png_for_texture(&img, &ww, &hh, filename)) {
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ww, hh, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, img));
    free(img);
    this->width = ww;
    this->height = hh;
  }
}

template <typename GRAPHICS>
class OpenglScreenBuffer : public OpenglBaseBuffer<GRAPHICS> {};
} // namespace Kvant::graphics::opengl

#endif // OPENGL_BUFFER_HPP_INCLUDED
