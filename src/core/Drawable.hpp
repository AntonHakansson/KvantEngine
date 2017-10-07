#ifndef DRAWABLE_HPP_INCLUDED
#define DRAWABLE_HPP_INCLUDED

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "core/Traits.hpp"
#include "core/Loader.hpp"
#include "core/Mesh.hpp"
#include "backends/blueprints/Graphics.hpp"

namespace Kvant {

    template <typename GRAPHICS, typename ACTUAL>
    class Drawable : public IsDrawable<ACTUAL> {
        public:
            Drawable(const GraphicsContext<GRAPHICS>& ctx, std::string filename,
                     std::function<std::unique_ptr<Model>(const char* filename)> loader = read_smd);
            
            virtual ~Drawable() {}

            template <typename PIPELINE>
            void draw_instances(const GraphicsContext<GRAPHICS>& ctx, const PIPELINE& s,
                                const typename PIPELINE::Material& material,
                                const glm::mat4* transforms, int count) const;

        private:
            std::vector<std::unique_ptr<Kvant::blueprints::graphics::Surface<GRAPHICS, typename GRAPHICS::Surface>>> _surfaces;
    };


    template <typename GRAPHICS, typename ACTUAL>
    Drawable<GRAPHICS, ACTUAL>::Drawable(const GraphicsContext<GRAPHICS>& ctx, std::string filename,
                                         std::function<std::unique_ptr<Model>(const char* filename)> loader) {
        auto model = loader(filename.c_str());
        for (auto& m : model->meshes) {
            auto s = std::make_unique<typename GRAPHICS::Surface>(ctx, m.get());
            _surfaces.push_back(std::move(s));
        }
    }


    template <typename GRAPHICS, typename ACTUAL>
    template <typename PIPELINE>
    void Drawable<GRAPHICS, ACTUAL>::draw_instances(const GraphicsContext<GRAPHICS>& ctx, const PIPELINE& s,
                                                const typename PIPELINE::Material& material,
                                                const glm::mat4* transforms, int count) const {
        material.bind(ctx, s);
        for (const auto& surf : _surfaces) {
            surf.get()->draw_instanced(*ctx.graphics, s, transforms, count);
        }
    }

}

#endif // DRAWABLE_HPP_INCLUDED