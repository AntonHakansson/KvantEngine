#ifndef CONTEXT_HPP_INCLUDED
#define CONTEXT_HPP_INCLUDED

#include <chrono>
#include <map>
#include "imgui/imgui.h"

#include "core/Scene.hpp"

namespace Kvant {
    struct CoreContext {
        float elapsed;
        std::function<void()> quit;

        struct Measurment {
            mutable long long _b;
            mutable long long _total;
            mutable int avg = 0;
            mutable long long _count = 0;

            void begin() const {
                _b = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            }

            void end() const {
                _count++;
                int d = std::chrono::high_resolution_clock::now().time_since_epoch().count() - _b;
                _total += d;
                avg = _total / _count;
            }
        };

        mutable std::map<std::string, Measurment> _m;

        const Measurment& measure(std::string what) const {
            return _m[what];
        }

        void imgui() const {
            ImGui::Separator();
            float total = 0;
            for (auto& m : _m) {
                float avg = m.second.avg;
                avg /= 1000000.0f;
                ImGui::Text("%s: %fms", m.first.c_str(), avg);
                total += avg;
            }
            ImGui::Text("TOTAL: %fms", total);
            ImGui::Separator(); 
        }
    };

    template <typename PLATFORM>
    struct PlatformContext {
        PLATFORM* p;
        const PLATFORM* get_platform() const {
            return p;
        }
    };

    template <typename GRAPHICS>
    struct GraphicsContext {
        GRAPHICS* graphics;
        const GRAPHICS* get_graphics() const {
            return graphics;
        }
    };

    template <typename PLATFORM, typename GRAPHICS>
    struct Context :    PlatformContext<PLATFORM>,
                        GraphicsContext<GRAPHICS>,
                        CoreContext {
        SceneManager<Context<PLATFORM, GRAPHICS>>* scenes;
    };
}

#endif // CONTEXT_HPP_INCLUDED