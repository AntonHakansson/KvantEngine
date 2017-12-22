#include "Kvant.hpp"

#include "backends/sdl2/sdl2.hpp"
#include "backends/opengl/Opengl.hpp"

using my_app = Kvant::App<Kvant::platform::sdl::BackendForOpengl,
                                  Kvant::graphics::opengl::Backend,
                                  Kvant::Context>;
class SpaceScene;
class IntroScene : public my_app::Scene {
    public:
        IntroScene(const my_app::Context &ctx)
          : _camera(ctx, Kvant::pos::look_at({10, 0, -20}, {0, 0, 0}), &ctx.p->screen()),
            _pipeline(ctx) {
                _quit = [ctx]() { ctx.quit(); };
            }

        void update(const my_app::Context& ctx) override {
            if (ctx.get_platform()->is_key_pressed(Kvant::key::Q)) {
                ctx.quit();
            }

            render(ctx);
        }

        void render(const my_app::Context& ctx) override {
            my_app::RenderPass(ctx)
                .target(my_app::Graphics::screen_buffer)
                .clear();
            
            _pipeline.use(ctx)
                .set_eyepos(_camera.entity().pos)
                .set_camera(_camera);
        }
            
        void update_scenes(const my_app::Context& ctx) override {
            if (_change_to_space)
                ctx.scenes->push_scene<SpaceScene>(ctx);
        }
        
        void resume(const my_app::Context& ctx) override { LOG_DEBUG << "MENu: Im Resumed!!"; }
        void pause(const my_app::Context& ctx) override { LOG_DEBUG << "MENU: Im Paused!!"; }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
            _change_to_space = false;
            if (ImGui::CollapsingHeader("Scenes")) {
                if (ImGui::Button("NEXT SCENE")) {
                    _change_to_space = true;
                }
            }

        }

    private:
        Kvant::Instance<Kvant::Camera> _camera;
        my_app::ForwardPipeline _pipeline;
        std::function<void()> _quit;
        bool _change_to_space{false};
};
    
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
        SpaceScene(const my_app::Context& ctx) 
            : _moon(ctx, Kvant::pos::origin),
              _camera{ctx, Kvant::PovDriver::look_at({0.0, 0.0, -50.0}, {0.0, 0.0, 0.0}), &ctx.p->screen()},
              _pipeline(ctx) {
        }

        void update(const my_app::Context& ctx) override {
            _wasd.update(ctx, _camera.get_driver_ptr());

            render(ctx);
        }

        void render(const my_app::Context& ctx) override {
            my_app::RenderPass(ctx)
                .target(my_app::Graphics::screen_buffer)
                .clear();
            
            _pipeline.use(ctx)
                .set_material(_moon.get_drawable().get_material())
                .set_eyepos(_camera.entity().pos)
                .set_camera(*_camera.drivable_ptr())
                .draw(_moon);
        }
            
        void update_scenes(const my_app::Context& ctx) override {
            if (ctx.get_platform()->is_key_pressed(Kvant::key::Q)) {
                ctx.scenes->pop_scene(ctx);
            }
        }

        void resume(const my_app::Context& ctx) override {
            LOG_DEBUG << "SPACE: Im paused!!";
        }
        void pause(const my_app::Context& ctx) override {
            LOG_DEBUG << "SPACE: Im paused!!";
        }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
        }

    private:
        Kvant::Driven<Kvant::Instance<Moon>, Kvant::DirectDriver> _moon;
        Kvant::Driven<Kvant::Instance<Kvant::Camera>, Kvant::HoverDriver> _camera;
        Kvant::wsad_controller  _wasd;
        my_app::ForwardPipeline _pipeline;
};

int main() {
    try {
        my_app().run<IntroScene>();
    } catch (const std::exception & e) {
        LOG_ERROR << e.what();
        return 1;
    }
}
