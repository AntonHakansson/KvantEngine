#include "Kvant.hpp"

#include "backends/sdl2/sdl2.hpp"
#include "backends/opengl/Opengl.hpp"

using my_app = Kvant::App<Kvant::platform::sdl::BackendForOpengl,
                                  Kvant::graphics::opengl::Backend,
                                  Kvant::Context>;

class Moon : public my_app::Asset<Moon>,
             public my_app::Drawable<Moon> {

    public:
        Moon(const my_app::Context& ctx) : my_app::Drawable<Moon>(ctx, "res/examples/moon.smd"),
                                           _diffuse_map(ctx, "res/examples/moon_d.png"),
                                           _normal_map(ctx, "res/examples/moon_nm.png"),
                                           _specular_map(ctx, "res/examples/moon_s.png"),
                                           _material(ctx, &_diffuse_map, &_normal_map, &_specular_map) {

        }

        const my_app::Material& get_material() const {
            return _material;
        }

    private:
        my_app::Texture _diffuse_map, _normal_map, _specular_map;
        my_app::Material _material;
};

class SpaceScene : public my_app::Scene {
    public:

        SpaceScene(const my_app::Context& ctx, Kvant::Screen* screen) : _moon(ctx), _pipeline(ctx) {

        }

        void update(const my_app::Context& ctx) override {

        }

        void render(const my_app::Context& ctx) override {

        }

    private:
        Kvant::Instance<Moon> _moon;
        my_app::ForwardPipeline _pipeline;
};

int main() {
    try {
        my_app().run<SpaceScene>();
    } catch (const std::exception & e) {
        LOG_ERROR << e.what();
    }
}