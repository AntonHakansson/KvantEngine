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
        SpaceScene(const my_app::Context& ctx, Kvant::Screen* screen) 
            : _moon(ctx, Kvant::pos::origin),
              _camera{ctx, Kvant::PovDriver::look_at({0.0, 0.0, -1.0}, {0.0, 0.0, 0.0}), screen},
              _pipeline(ctx) {
        }

        void update(const my_app::Context& ctx) override {
            if (ctx.get_platform()->is_key_pressed(Kvant::key::Q)) {
                ctx.quit();
            }
            /*if (ctx.get_platform()->is_key_pressed(Kvant::key::A)) {
                auto moon_trans = _moon.get_transforms_ptr();
                *moon_trans = glm::translate(*moon_trans, glm::vec3(0.01, 0, 0));
            }
            if (ctx.get_platform()->is_key_pressed(Kvant::key::D)) {
                auto moon_trans = _moon.get_transforms_ptr();
                *moon_trans = glm::translate(*moon_trans, glm::vec3(-0.01, 0, 0));
            }*/

            _wasd.update(ctx, _camera.get_driver_ptr());

            render(ctx);
        }

        void render(const my_app::Context& ctx) override {
            my_app::RenderPass(ctx)
                .target(my_app::Graphics::screen_buffer)
                .clear({0, 0, 0, 1});
            
            _pipeline.use(ctx)
                .set_material(_moon.get_drawable().get_material())
                .set_camera(*_camera.drivable_ptr())
                .draw(_moon);
        }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
        }

        private : Kvant::Instance<Moon> _moon;
        Kvant::Driven<Kvant::Instance<Kvant::Camera>, Kvant::HoverDriver> _camera;
        Kvant::wsad_controller  _wasd;
        my_app::ForwardPipeline _pipeline;
};

int main() {
    try {
        my_app().run<SpaceScene>();
    } catch (const std::exception & e) {
        LOG_ERROR << e.what();
        return 1;
    }
}
