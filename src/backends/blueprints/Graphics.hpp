#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <glm/glm.hpp>

#include "core/Context.hpp"
#include "core/Mesh.hpp"

namespace Kvant::blueprints::graphics {

template <typename B> class Pipeline {};

template <typename GRAPHICS, typename SURFACE> struct Surface {
  int n_vertices;
  int n_triangles;
  glm::vec3 max_v;
  glm::vec3 min_v;

  Surface(const GraphicsContext<typename GRAPHICS::Backend> &ctx, Mesh *m) {}
  virtual ~Surface() {}

  void calc_bounds(const Mesh *m) {
    float max_x, min_x, max_y, min_y, max_z, min_z;
    max_x = min_x = max_y = min_y = max_z = min_z = 0;
    for (const auto &v : m->vertices) {
      glm::vec3 p = v.position;
      max_x = std::max(max_x, p.x);
      min_x = std::min(min_x, p.x);
      max_y = std::max(max_y, p.y);
      min_y = std::min(min_y, p.y);
      max_z = std::max(max_z, p.z);
      min_z = std::min(min_z, p.z);
    }
    max_v = glm::vec3(max_x, max_y, max_z);
    min_v = glm::vec3(min_x, min_y, min_z);
  }

  template <typename PIPELINE>
  void draw_instanced(const GRAPHICS &backend, const PIPELINE &shader,
                      const glm::mat4 *transforms, int count) const {
    static_cast<const SURFACE *>(this)->draw_instanced(backend, shader,
                                                       transforms, count);
  }
};

template <typename BACKEND> class Backend {
public:
  Backend() {
    static_assert(
        std::is_base_of<Pipeline<BACKEND>, typename BACKEND::Pipeline>::value);
    static_assert(std::is_base_of<Pipeline<BACKEND>,
                                  typename BACKEND::BasePipeline>::value);
    static_assert(std::is_base_of<Surface<BACKEND, typename BACKEND::Surface>,
                                  typename BACKEND::Surface>::value);
  }
  virtual void clear_screen() const = 0;
  virtual void update_frame() = 0;
};
} // namespace Kvant::blueprints::graphics

#endif // GRAPHICS_HPP_INCLUDED
