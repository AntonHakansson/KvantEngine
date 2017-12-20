#include "Kvant.hpp"

#include "backends/sdl2/sdl2.hpp"
#include "backends/opengl/Opengl.hpp"

using my_app = Kvant::App<Kvant::platform::sdl::BackendForOpengl,
                          Kvant::graphics::opengl::Backend,
                          Kvant::Context>;

class EmptyScene : public my_app::Scene {
    public:
        EmptyScene(const my_app::Context& ctx, Kvant::Screen* screen) { 

        }

        void update(const my_app::Context& ctx) override {
            LOG_DEBUG << "SCENE UPDATE";
        }

        void render(const my_app::Context& ctx) override {
            LOG_DEBUG << "SCENE REDNER ";
        }
};

int main() {
    try {
        return std::make_unique<my_app>()->run<EmptyScene>();
    } catch (const std::exception& e) {
        LOG_ERROR << e.what();
        return 1;
    }
}