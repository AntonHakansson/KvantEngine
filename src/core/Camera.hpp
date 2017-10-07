#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include <glm/glm.hpp>

#include "core/Screen.hpp"
#include "core/Traits.hpp"

namespace Kvant {
    struct Pov {
        glm::vec3 pos;  // Position of view
        glm::vec3 tgt;  // position of target
        glm::mat4 proj; // projection matrix
    };

    class Camera : public Pov,
                    private Screen::Subscriber,
                    public IsEntity<Camera>,
                    public IsDrivable<Camera> {

        public:
            template <typename CONTEXT>
            Camera(const CONTEXT& ctx, Screen* screen, float fov = 0.28f, float near = 0.1f, float far = 1024.0f) : _fov(fov), _near(near), _far(far){
                this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
                this->proj = glm::mat4();
                this->tgt = glm::vec3();
                screen->subscribe(*this);
            }
            void imgui();

        protected:
            float _fov, _near, _far;

        private:
            void on_screen_resize(unsigned int width, unsigned int height);
    };
}

#endif // CAMERA_HPP_INCLUDED