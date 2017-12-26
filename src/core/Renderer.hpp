#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include <glm/glm.hpp>

namespace Kvant {
using namespace glm;

template <typename GRAPHICS, typename RENDERER> class Renderer {
public:
  template <typename... PIPELINE>
  void record(const GraphicsContext<GRAPHICS> &ctx,
              std::function<void(const PIPELINE &...)> cmds) const {
    static_cast<const Renderer *>(this)->record(ctx, cmds);
  }
};

template <typename GRAPHICS> class RenderPass {

public:
  RenderPass(const GraphicsContext<GRAPHICS> &ctx) : m_ctx(ctx) {}

  const RenderPass &target(const typename GRAPHICS::FrameBuffer &fb) const {
    fb.bind();
    return *this;
  }

  const RenderPass &clear(vec4 color = {0.9, 0.9, 0.9, 1}) const {
    GRAPHICS::clear.apply(color);
    return *this;
  }

private:
  const GraphicsContext<GRAPHICS> &m_ctx;
};

} // namespace Kvant

#endif // RENDERER_HPP_INCLUDED
