#ifndef DRAWABLE_HPP_INCLUDED
#define DRAWABLE_HPP_INCLUDED

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "backends/blueprints/Graphics.hpp"
#include "core/Loader.cpp"
#include "core/Mesh.hpp"
#include "core/Traits.hpp"

namespace Kvant {

template <typename GRAPHICS, typename ACTUAL>
class Drawable : public IsDrawable<ACTUAL> {
public:
  Drawable(const GraphicsContext<GRAPHICS> &ctx, std::string filename) {
    auto model_data = read_model(filename.c_str());
    load_model_data(ctx, model_data);
  }
  Drawable(const GraphicsContext<GRAPHICS> &ctx,
           const RawModelData &model_data) {
    load_model_data(ctx, model_data);
    LOG_DEBUG << "Successfully loaded data " << model_data.diffuses.size()
              << ", " << model_data.normals.size() << ", "
              << model_data.speculars.size();
  }

  virtual ~Drawable() {}

  template <typename PIPELINE>
  void draw_instances(const GraphicsContext<GRAPHICS> &ctx, const PIPELINE &s,
                      const typename PIPELINE::Material &material,
                      const glm::mat4 *transforms, int count) const;

  template <typename PIPELINE>
  void draw_instances(const GraphicsContext<GRAPHICS> &ctx, const PIPELINE &s,
                      const glm::mat4 *transforms, int count) const;

private:
  void load_model_data(const GraphicsContext<GRAPHICS> &ctx,
                       const RawModelData &model_data) {
    for (auto &m : model_data.model.meshes) {
      auto s = std::make_unique<typename GRAPHICS::Surface>(ctx, m.get());
      _surfaces.push_back(std::move(s));
    }
  }
  std::vector<std::unique_ptr<Kvant::blueprints::graphics::Surface<
      GRAPHICS, typename GRAPHICS::Surface>>>
      _surfaces;
};

template <typename GRAPHICS, typename ACTUAL>
template <typename PIPELINE>
void Drawable<GRAPHICS, ACTUAL>::draw_instances(
    const GraphicsContext<GRAPHICS> &ctx, const PIPELINE &s,
    const typename PIPELINE::Material &material, const glm::mat4 *transforms,
    int count) const {
  material.bind(ctx, s);
  for (const auto &surf : _surfaces) {
    surf.get()->draw_instanced(*ctx.graphics, s, transforms, count);
  }
}

template <typename GRAPHICS, typename ACTUAL>
template <typename PIPELINE>
void Drawable<GRAPHICS, ACTUAL>::draw_instances(
    const GraphicsContext<GRAPHICS> &ctx, const PIPELINE &s,
    const glm::mat4 *transforms, int count) const {
  for (const auto &surf : _surfaces) {
    surf.get()->draw_instanced(*ctx.graphics, s, transforms, count);
  }
}
} // namespace Kvant

#endif // DRAWABLE_HPP_INCLUDED
