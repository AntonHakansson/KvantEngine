#include "core/Screen.hpp"
#include "backends/sdl2/sdl2.hpp"
#include "imgui/imgui.h"
#include "backends/sdl2/imgui_impl_sdl.h"
#include "utils/Checks.hpp"

namespace Kvant::platform::sdl {

    bool PlatformIsSdl::is_key_pressed(key k) const {
        return _kbstate[YAGL_TO_SDL_KEY[k]];
    }

    void PlatformIsSdl::update_keyboard() {
        _kbstate = SDL_GetKeyboardState(NULL);
    }

    void PlatformIsSdl::update_mouse() {
        mstate = SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    }

    void PlatformIsSdl::get_mouse(int* x, int* y) const {
        *x = mouse_x;
        *y = mouse_y;
    }

    bool PlatformIsSdl::is_button_pressed() const {
        return (mstate & SDL_BUTTON(1));
    }

    bool PlatformIsSdl::capture_mouse() const {
        if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::GetIO().WantCaptureMouse) {
            SDL_SetRelativeMouseMode (SDL_TRUE);
            return true;
        } else return false;
    }
    
    void PlatformIsSdl::release_mouse() const {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    PlatformIsSdl::PlatformIsSdl(Uint32 ext_flags) {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            std::runtime_error("SDL Initialization error");
        }
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        _screen.w = (int)(mode.h - 200) / 1.5;
        _screen.h = mode.h - 200;
        float zoom = 1.0f;
        _main_window = SDL_CreateWindow("Sample Window", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, _screen.w, _screen.h,
                                        ext_flags | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!_main_window) {
            std::runtime_error("Unable to create window");
        }
    }

    PlatformIsSdl::~PlatformIsSdl() {
        SDL_DestroyWindow(_main_window);
        SDL_Quit();
    }

    bool PlatformIsSdl::process_events() {
        bool done = false;
        SDL_Event event;
        while (SDL_PollEvent(&event) && !done) {
            if (ImGui_ImplSdl_ProcessEvent(&event)) continue;
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_WINDOWEVENT: 
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            on_window_resize(&event);
                            break;
                    }
                case SDL_KEYDOWN:
                    on_key_callback(event.key.keysym.sym);
                    break;
            }
        }
        return !done;
    }

    void PlatformIsSdl::create_window() {

    }

    std::string PlatformIsSdl::read_file(std::string filename) {
        std::string buf;
        SDL_RWops* sdlIO = SDL_RWFromFile(filename.c_str(), "r");
        if (sdlIO != NULL) {
            char data;
            std::size_t amountRead;
            while ((amountRead = SDL_RWread(sdlIO, &data, 1, 1)) > 0) {
                buf += data;
            }

            SDL_RWclose(sdlIO);
        }
        return buf;
    }

    BackendForOpengl::BackendForOpengl() : PlatformIsSdl(SDL_WINDOW_OPENGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        _gl_context = SDL_GL_CreateContext(_main_window);
        if(!_gl_context) {
            std::runtime_error("Unable to create GL context");
        }

        ImGui_ImplSdl_Init(_main_window);
        glViewport(0, 0, _screen.w, _screen.h);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(_main_window);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glEnable(GL_DEPTH_TEST));
    }

    BackendForOpengl::~BackendForOpengl() {
        ImGui_ImplSdl_Shutdown();
        SDL_GL_DeleteContext(_gl_context);
    }

    void BackendForOpengl::on_window_resize(SDL_Event const* event) {
        SDL_GL_GetDrawableSize(_main_window, &_screen.w, &_screen.h);
        on_resize_callback(_screen.w, _screen.h);
    }

    void BackendForOpengl::update_window() {
        SDL_GL_SwapWindow(_main_window);
    }

    void BackendForOpengl::imgui_frame() {
        glActiveTexture(GL_TEXTURE0);
        //glUseProgram(0);
        ImGui_ImplSdl_NewFrame(_main_window);
    }

}