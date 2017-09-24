#ifndef SDL2_HPP_INCLUDED
#define SDL2_HPP_INCLUDED

#include <string>
#include <type_traits>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles.h>

#include "Platform.hpp"

namespace Kvant::platform::sdl {
    static int YAGL_TO_SDL_KEY[1024] =
        {[key::A] = SDL_SCANCODE_A,          [key::B] = SDL_SCANCODE_B,
        [key::C] = SDL_SCANCODE_C,          [key::D] = SDL_SCANCODE_D,
        [key::E] = SDL_SCANCODE_E,          [key::F] = SDL_SCANCODE_F,
        [key::G] = SDL_SCANCODE_G,          [key::H] = SDL_SCANCODE_H,
        [key::I] = SDL_SCANCODE_I,          [key::J] = SDL_SCANCODE_J,
        [key::K] = SDL_SCANCODE_K,          [key::L] = SDL_SCANCODE_L,
        [key::M] = SDL_SCANCODE_M,          [key::N] = SDL_SCANCODE_N,
        [key::O] = SDL_SCANCODE_O,          [key::P] = SDL_SCANCODE_P,
        [key::Q] = SDL_SCANCODE_Q,          [key::R] = SDL_SCANCODE_R,
        [key::S] = SDL_SCANCODE_S,          [key::T] = SDL_SCANCODE_T,
        [key::U] = SDL_SCANCODE_U,          [key::V] = SDL_SCANCODE_V,
        [key::W] = SDL_SCANCODE_W,          [key::X] = SDL_SCANCODE_X,
        [key::Y] = SDL_SCANCODE_Y,          [key::Z] = SDL_SCANCODE_Z,
        [key::N1] = SDL_SCANCODE_1,         [key::N2] = SDL_SCANCODE_2,
        [key::N3] = SDL_SCANCODE_3,         [key::N4] = SDL_SCANCODE_4,
        [key::N5] = SDL_SCANCODE_5,         [key::N6] = SDL_SCANCODE_6,
        [key::N7] = SDL_SCANCODE_7,         [key::N8] = SDL_SCANCODE_8,
        [key::N9] = SDL_SCANCODE_9,         [key::N0] = SDL_SCANCODE_0,
        [key::UP] = SDL_SCANCODE_UP,        [key::DOWN] = SDL_SCANCODE_DOWN,
        [key::LEFT] = SDL_SCANCODE_LEFT,    [key::RIGHT] = SDL_SCANCODE_RIGHT,
        [key::ENTER] = SDL_SCANCODE_RETURN, [key::TAB] = SDL_SCANCODE_TAB,
        [key::ESC] = SDL_SCANCODE_ESCAPE,   [key::BACKSPACE] = SDL_SCANCODE_BACKSPACE,
        [key::SPACE] = SDL_SCANCODE_SPACE};
    
    class SdlWithOpengl {};
    
    class PlatformIsSdl : public blueprints::platform::Backend {
        public:
            PlatformIsSdl(Uint32 ext_flags);
            virtual ~PlatformIsSdl();
            
            void create_window() override;
            void update_keyboard() override;
            bool capture_mouse() const override;
            void release_mouse() const override;
            void update_mouse() override;
            bool process_events() override;
            bool is_key_pressed(key k) const override;
            bool is_button_pressed() const override;
            void get_mouse(int *x, int *y) const override;

            virtual void on_window_resize(SDL_Event const* event) = 0;
            static std::string read_file(std::string filename);

        protected:
            SDL_Window* _main_window;
            SDL_GLContext _gl_context;
            const Uint8* _kbstate;
            int mouse_x, mouse_y;
            Uint8 mstate;
    };

    class BackendForOpengl : public PlatformIsSdl {
        public:
            BackendForOpengl();
            virtual ~BackendForOpengl();

            void update_window() override;
            void on_window_resize(SDL_Event const* event) override;
            void imgui_frame();

        private:
            SDL_GLContext _gl_context;
    };

}

#endif // SDL2_HPP_INCLUDED