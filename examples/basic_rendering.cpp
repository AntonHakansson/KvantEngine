#include "Kvant.hpp"

#include "backends/sdl2/sdl2.hpp"
#include "backends/opengl/Opengl.hpp"
#include "core/Loader.hpp"

using my_app = Kvant::App<Kvant::platform::sdl::BackendForOpengl,
                                  Kvant::graphics::opengl::Backend,
                                  Kvant::Context>;
class SpaceScene;
class CrateScene;
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
            if (_change_to_space) ctx.scenes->push_scene<SpaceScene>(ctx);
            if (_change_to_crate) ctx.scenes->push_scene<CrateScene>(ctx);
        }
        
        void resume(const my_app::Context& ctx) override { LOG_DEBUG << "MENu: Im Resumed!!"; }
        void pause(const my_app::Context& ctx) override { LOG_DEBUG << "MENU: Im Paused!!"; }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
            _change_to_space = false;
            if (ImGui::CollapsingHeader("Example Scenes")) {
                _change_to_space = ImGui::Button("MOON");
                _change_to_crate = ImGui::Button("CRATE");
            }
        }

    private:
        Kvant::Instance<Kvant::Camera> _camera;
        my_app::ForwardPipeline _pipeline;
        std::function<void()> _quit;
        bool _change_to_space{false}, _change_to_crate{false};
};
    
Kvant::RawModelData _model_data(Kvant::read_model("res/examples/moon.fbx"));
std::string resDir = "res/examples/";
class Moon : public my_app::Asset<Moon>,
             public my_app::Drawable<Moon> {

    public:
        Moon(const my_app::Context& ctx) : my_app::Drawable<Moon>(ctx, _model_data),
                                           _diffuse_map(ctx, resDir + _model_data.diffuses[0]),
                                           _normal_map(ctx, resDir + _model_data.normals[0]),
                                           _specular_map(ctx, resDir + _model_data.speculars[0]),
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
            if (_pop_scene) {
                ctx.scenes->pop_scene(ctx);
            }
        }

        void resume(const my_app::Context& ctx) override {
            LOG_DEBUG << "SPACE: Im resumed!!";
        }
        void pause(const my_app::Context& ctx) override {
            LOG_DEBUG << "SPACE: Im paused!!";
        }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
            _pop_scene = ImGui::Button("Return to scene select");
        }

    private:
        Kvant::Driven<Kvant::Instance<Moon>, Kvant::DirectDriver> _moon;
        Kvant::Driven<Kvant::Instance<Kvant::Camera>, Kvant::HoverDriver> _camera;
        Kvant::wsad_controller  _wasd;
        my_app::ForwardPipeline _pipeline;

        bool _pop_scene{false};
};

class Crate : public my_app::Asset<Crate>,
              public my_app::Drawable<Crate>
{
  private:
    my_app::Texture _diffuse_map, _normal_map, _specular_map;
    my_app::Material _material;

  public:
    Crate(const my_app::Context &ctx)
        : my_app::Drawable<Crate>{ctx, "res/examples/crate2.smd"},
          _diffuse_map{ctx, "res/examples/crate2_d.png"},
          _normal_map{ctx, "res/examples/crate2_n.png"},
          _specular_map{ctx, "res/examples/crate2_s.png"},
          _material{ctx, &_diffuse_map, &_normal_map, &_specular_map}
    {
    }
    const my_app::Material &get_material() const
    {
        return _material;
    }
};

class CrateScene : public my_app::Scene {
    public:
        CrateScene(const my_app::Context& ctx) 
            : _camera{ctx, Kvant::PovDriver::look_at({0.0, 0.0, -50.0}, {0.0, 0.0, 0.0}), &ctx.p->screen()},
              _pipeline(ctx),
              _crate(ctx, Kvant::pos::origin) {
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
                .set_camera(*_camera.drivable_ptr())
                .set_material(_crate.get_drawable().get_material())
                .draw(_crate);
        }

        void update_scenes(const my_app::Context& ctx) override {
            if (_pop_scene) {
                ctx.scenes->pop_scene(ctx);
            }
        }

        void resume(const my_app::Context& ctx) override {
            LOG_DEBUG << "Crate: Im Resumed!!";
        }
        void pause(const my_app::Context& ctx) override {
            LOG_DEBUG << "Crate: Im paused!!";
        }

        void imgui(const my_app::Context &ctx) override {
            _camera.entity_ptr()->imgui();
            _pop_scene = ImGui::Button("Return to scene select");
        }

    private:
        my_app::ForwardPipeline _pipeline;
        Kvant::Driven<Kvant::Instance<Kvant::Camera>, Kvant::HoverDriver> _camera;
        Kvant::wsad_controller _wasd;

        Kvant::Instance<Crate> _crate;
        bool _pop_scene{false};
 };
    
int main() {
    try {
        my_app().run<IntroScene>();
    } catch (const std::exception & e) {
        LOG_ERROR << e.what();
        return 1;
    }
}
