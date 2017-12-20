#ifndef PLATFORM_HPP_INCLUDED
#define PLATFORM_HPP_INCLUDED

#include <string>
#include <cstring>
#include <fstream>
#include <functional>
#include <errno.h>

#include "core/Screen.hpp"
#include "utils/Logger.hpp"

namespace Kvant {
    enum key {
        A=0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        N1, N2, N3, N4, N5, N6, N7, N8, N9, N0, 
        UP, DOWN, LEFT, RIGHT,
        ENTER, TAB, ESC, BACKSPACE, SPACE 
    };

    namespace blueprints::platform {
        class Backend {
            public:
                Kvant::Screen _screen;
                virtual void create_window() = 0;
                virtual void update_window() = 0;
                virtual void update_keyboard() = 0;
                virtual bool capture_mouse() const = 0;
                virtual void release_mouse() const = 0;
                virtual void update_mouse() = 0;
                virtual bool process_events() = 0;
                virtual bool is_key_pressed(key k) const = 0;
                virtual bool is_button_pressed() const = 0;
                virtual void get_mouse(int *x, int *y) const = 0;
                Kvant::Screen& screen() { return _screen; }
                std::function<void(int w, int h)> on_resize_callback;
                std::function<void(int k)> on_key_callback;

                static std::string read_file(std::string filename) {
                    std::ifstream in;
                    in.open(filename, std::ifstream::in);

                    if(!in) {
                        LOG_ERROR << "Error reading file: " << filename;
                        LOG_ERROR << strerror(errno);
                        return "";
                    }

                    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
                }
        };
    }

}

#endif // PLATFORM_HPP_INCLUDED