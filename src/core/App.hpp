#ifndef APP_HPP_INCLUDED
#define APP_HPP_INCLUDED

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "backends/blueprints/Platform.hpp"
#include "backends/blueprints/Graphics.hpp"

#include "utils/Logger.hpp"
#include "core/Scene.hpp"
#include "imgui/imgui_kvant.cpp"

namespace Kvant {
    template <typename PLATFORM,
              template <typename> typename GRAPHICS,
              template <typename, typename> typename CONTEXT>
    class App {
        public:
            static_assert(std::is_base_of<Kvant::blueprints::platform::Backend, PLATFORM>::value,
                        "platform");
            using Platform = PLATFORM;

            static_assert(std::is_base_of<Kvant::blueprints::graphics::Backend<GRAPHICS<PLATFORM>>, GRAPHICS<Platform>>::value,
                        "platform");
            using Graphics = GRAPHICS<Platform>;

            using Context = CONTEXT<Platform, Graphics>;

            using Scene = Kvant::Scene<Context>;

            using empty_scene = Kvant::empty_scene<Context>;

            App() : _graphics(&_platform, &_platform._screen) {
                LOG_DEBUG << "Start of Application.";
                
                _platform.on_resize_callback = [this](int w, int h) {
                    _platform._screen.notify_Subscribers();
                    _active_scene->on_screen_resize(this->_ctx);
                };
                _platform.on_key_callback = [this](int k) { this->on_key(k); };
                
                _ctx.quit = [this]() { this->quit(); };
                _ctx.p = &_platform;
                _ctx.graphics = &_graphics;

                set_imgui_style();
            }

            virtual ~App() {
                LOG_DEBUG << "App Cleanup.";
            }

            template <typename FIRST_SCENE>
            int run() {
                LOG_DEBUG << "Entering Main Game Loop.";

                _active_scene = std::make_unique<FIRST_SCENE>(_ctx, &_platform._screen);
                std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
                start = std::chrono::high_resolution_clock::now();
                _ctx.elapsed = 0;
                while(_platform.process_events() && !_quit) {
                    _ctx.measure("core updates").begin();
                    _graphics.update_frame();
                    _platform.update_window();
                    _platform.update_keyboard();
                    _platform.update_mouse();
                    _ctx.measure("core updates").end();
                    this->update(_ctx);
                    end = std::chrono::high_resolution_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
                    float dt = ms.count() / 1000000.0f;
                    _ctx.elapsed = dt;
                    start = std::chrono::high_resolution_clock::now();
                }

                LOG_DEBUG << "Main Loop Reached End";
                return 0;
            }

        private:
            virtual void update(const Context& _ctx) {
                clear_frame();
                _active_scene.get()->update(_ctx);
                _ctx.measure("core imgui").begin();
                _ctx.p->imgui_frame();
                _active_scene.get()->imgui(_ctx);
                ImGui::Render();
                _ctx.measure("core imgui").end();
            }

            virtual void on_key(int k) { }

            void quit() {
                _quit = true;
            }

            void clear_frame() const {
                _graphics.clear_screen();
            }

            Platform _platform;
            Graphics _graphics;
            Context _ctx;
            std::unique_ptr<Scene> _active_scene;
            bool _quit = false;
    };

}

#endif // APP_HPP_INCLUDED