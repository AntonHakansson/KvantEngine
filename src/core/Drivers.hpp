#ifndef DRIVERS_HPP_INCLUDED
#define DRIVERS_HPP_INCLUDED

#include <glm/glm.hpp>

#include "core/Context.hpp"
#include "core/Camera.hpp"

namespace Kvant {

    /**
    * drivers are attached to instances and references and act
    * as controllers to manipulate their transformations.
    */
    class Driver {
        public:
            virtual glm::mat4 get_transform() const = 0;
            virtual void update() = 0;
            virtual void reuse() {};
    };

    class PovDriver : public Driver {
        public:
            struct initializer {
                glm::vec3 pos = {0.0, 0.0, -1.0};
                glm::vec3 tgt = {0.0, 0.0, 0.0};
            };

            static std::function<initializer(int)> look_at(glm::vec3 pos, glm::vec3 tgt) {
                return [pos, tgt](int j) -> initializer {
                    return initializer{pos, tgt};
                };
            }

            virtual void dolly(float d) = 0;
            virtual void truck(float d) = 0;
            virtual void follow(float a1, float a2) = 0;
            virtual void pan(float a1) = 0;
            virtual void jump() = 0;
            virtual void stop() = 0;
            virtual void pedestal(float d) = 0;
    };
    
    template <typename DRIVABLE>
    class HoverDriver : public PovDriver {
        public:
            HoverDriver(const CoreContext &context, glm::mat4 *transform, Pov *pov, PovDriver::initializer i)
                :   _driven_transform(transform), _pov(pov), _driven_pos(&pov->pos), _driven_tgt(&pov->tgt) {
                *_driven_pos = i.pos;
                *_driven_tgt = i.tgt;
                retransform();
            }

            virtual void dolly(float d) override {
                auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
                auto deltaPos = cam_dir * d;
                *_driven_pos += deltaPos;
                *_driven_tgt += deltaPos;
                retransform();
            }
            virtual void truck(float d) override {
                auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
                auto side_dir = glm::cross(cam_dir, _up);
                auto deltaPos = side_dir * d;
                *_driven_pos += deltaPos;
                *_driven_tgt += deltaPos;
                retransform();
            }
            virtual void follow(float a1, float a2) override {
                auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
                cam_dir.y += -a2;
                auto side_dir = glm::cross(cam_dir, _up);
                cam_dir += side_dir * -a1;
                cam_dir = glm::normalize(cam_dir);
                *_driven_tgt = *_driven_pos + cam_dir;
                retransform();
            }
            virtual void pan(float a1) override {

            }
            virtual void jump() override {

            }
            virtual void stop() override {

            }
            virtual void pedestal(float d) override {

            }

            virtual void update() {
                retransform();
            }

            virtual glm::mat4 get_transform() const override {
                return *_driven_transform;
            }

        private:
            glm::vec3* _driven_pos;
            glm::vec3* _driven_tgt;
            glm::mat4* _driven_transform;
            glm::vec3 _up{0.0, 1.0, 0.0};
            Pov* _pov;

            void retransform() {
                *_driven_transform = glm::lookAt(*_driven_pos,
                                                 *_driven_tgt,
                                                 _up);
            }
    };


    /**
     * CONTROLLERS
    */
    class wsad_controller {
        public:
            template <typename DRIVER, typename P>
            void update(const Kvant::PlatformContext<P> &ctx, DRIVER *pi, bool stop_on_nothing = true) {
                bool press = false;
                if (ctx.get_platform()->is_key_pressed(Kvant::key::SPACE)) {
                    pi->jump();
                    press = true;
                }
                if (ctx.get_platform()->is_key_pressed(Kvant::key::W)) {
                    pi->dolly(1);
                    press = true;
                }
                if (ctx.get_platform()->is_key_pressed(Kvant::key::S)) {
                    pi->dolly(-1);
                    press = true;
                }
                if (ctx.get_platform()->is_key_pressed(Kvant::key::D)) {
                    pi->truck(1);
                    press = true;
                }
                if (ctx.get_platform()->is_key_pressed(Kvant::key::A)) {
                    pi->truck(-1);
                    press = true;
                }
                if (ctx.get_platform()->is_key_pressed(Kvant::key::ESC)) {
                    _mouse_active = false;
                    ctx.get_platform()->release_mouse();
                }
                if (ctx.get_platform()->is_button_pressed()) {
                    if (ctx.get_platform()->capture_mouse()) {
                        _mouse_active = true;
                    }
                }
                if (!press && stop_on_nothing)
                    pi->stop();

                int mouse_x, mouse_y;
                ctx.get_platform()->get_mouse(&mouse_x, &mouse_y);
                if (_mouse_active) {
                    float a1 = -(float)mouse_x * 0.001;
                    float a2 = (float)mouse_y * 0.001;
                    pi->follow(a1, a2);
                }
            }

        private:
            bool _mouse_active{false};
    };
}

#endif  // DRIVERS_HPP_INCLUDED