#include "Kvant.hpp"

#include "backends/sdl2/sdl2.hpp"
#include "backends/opengl/Opengl.hpp"

using my_app = Kvant::App<Kvant::platform::sdl::BackendForOpengl,
                          Kvant::graphics::opengl::Backend,
                          Kvant::Context>;

int main() {
    try {
        return std::make_unique<my_app>()->run<my_app::empty_scene>();
    } catch (const std::exception& e) {
        LOG_ERROR << e.what();
        return 1;
    }
}