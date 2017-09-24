#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include "backends/blueprints/Platform.hpp"
#include "core/Screen.hpp"

namespace Kvant {
    template <typename CONTEXT>
    class Scene {
        public:
            Scene() {}
            virtual ~Scene() {}

            virtual void update(const CONTEXT& ctx) = 0;

            virtual void render(const CONTEXT& ctx) = 0;

            virtual void imgui(const CONTEXT& ctx){};

            virtual void on_screen_resize(const CONTEXT& ctx) {
                render(ctx);
            }

        private:

    };

    template <typename CONTEXT>
    class empty_scene : public Scene<CONTEXT> {
        public:
        empty_scene(const CONTEXT& ctx, Kvant::Screen* screen) {
        }
        virtual ~empty_scene() {}
        void update(const CONTEXT& ctx) override {
            if (ctx.get_platform()->is_key_pressed(key::ESC)) {
                ctx.quit();
            }
        }
        void render(const CONTEXT& ctx) override {
        }
    };
}

#endif // SCENE_HPP_INCLUDED 