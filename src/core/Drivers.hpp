#ifndef DRIVERS_HPP_INCLUDED
#define DRIVERS_HPP_INCLUDED

#include <glm/glm.hpp>

#include "core/Camera.hpp"
#include "core/Context.hpp"

namespace Kvant {

/**
 * drivers are attached to instances and references and act
 * as controllers to manipulate their transformations.
 */
class Driver {
public:
  virtual glm::mat4 get_transform() const = 0;
  virtual void update() = 0;
  virtual void reuse(){};
};

class PovDriver : public Driver {
public:
  struct initializer {
    glm::vec3 pos = {0.0, 0.0, -1.0};
    glm::vec3 tgt = {0.0, 0.0, 0.0};
  };

  static std::function<initializer(int)> look_at(glm::vec3 pos, glm::vec3 tgt) {
    return [pos, tgt](int j) -> initializer { return initializer{pos, tgt}; };
  }

  virtual void dolly(float d) = 0;
  virtual void truck(float d) = 0;
  virtual void follow(float a1, float a2) = 0;
  virtual void pan(float a1) = 0;
  virtual void jump() = 0;
  virtual void stop() = 0;
  virtual void strafe(float d) = 0;
};

template <typename DRIVABLE> class HoverDriver : public PovDriver {
public:
  HoverDriver(const CoreContext &context, glm::mat4 *transform, Pov *pov,
              PovDriver::initializer i)
      : _driven_transform(transform), _pov(pov), _driven_pos(&pov->pos),
        _driven_tgt(&pov->tgt) {
    *_driven_pos = i.pos;
    *_driven_tgt = i.tgt;
    retransform();
  }

  virtual void dolly(float d) override;
  virtual void truck(float d) override;
  virtual void follow(float a1, float a2) override;
  virtual void pan(float a1) override;
  virtual void jump() override {}
  virtual void stop() override {}
  virtual void strafe(float d) override;

  virtual void update() { retransform(); }

  virtual glm::mat4 get_transform() const override {
    return *_driven_transform;
  }

private:
  glm::vec3 *_driven_pos;
  glm::vec3 *_driven_tgt;
  glm::mat4 *_driven_transform;
  glm::vec3 _up{0.0, 1.0, 0.0};
  Pov *_pov;

  void retransform() {
    *_driven_transform = glm::lookAt(*_driven_pos, *_driven_tgt, _up);
  }
};

template <typename DRIVABLE> class DirectDriver {
public:
  using initializer = glm::mat4;
  DirectDriver(const CoreContext &ctx, glm::mat4 *transform, DRIVABLE *d,
               glm::mat4 im)
      : _driven_transform(transform) {
    reset();
  }

  void update();
  void reset();
  void rotate(glm::vec3 v);
  void rotate(glm::quat v);
  void look_at(glm::vec3 pos, glm::vec3 tgt,
               glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));
  void scale(glm::vec3 v);
  void translate(glm::vec3 v);

  glm::mat4 *direct() { return _driven_transform; }

private:
  glm::mat4 *_driven_transform;
  glm::mat4 _transform;

  glm::vec3 _translation;
  glm::mat4 _rotation;
  glm::vec3 _scale;
};

/**
 * CONTROLLERS
 */
class wsad_controller {
public:
  template <typename DRIVER, typename P>
  void update(const Kvant::PlatformContext<P> &ctx, DRIVER *pi,
              bool stop_on_nothing = true) {
    bool press = false;
    if (ctx.get_platform()->is_key_down(Kvant::key::SPACE)) {
      pi->jump();
      press = true;
    }
    if (ctx.get_platform()->is_key_down(Kvant::key::W)) {
      pi->dolly(1);
      press = true;
    }
    if (ctx.get_platform()->is_key_down(Kvant::key::S)) {
      pi->dolly(-1);
      press = true;
    }
    if (ctx.get_platform()->is_key_down(Kvant::key::D)) {
      pi->truck(1);
      press = true;
    }
    if (ctx.get_platform()->is_key_down(Kvant::key::A)) {
      pi->truck(-1);
      press = true;
    }
    if (ctx.get_platform()->is_key_down(Kvant::key::ESC)) {
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
      float a1 = -(float)mouse_x * _mouse_sensitivity / 1000;
      float a2 = (float)mouse_y * _mouse_sensitivity / 1000;
      pi->follow(a1, a2);
    }
  }

private:
  bool _mouse_active{false};
  float _mouse_sensitivity{2.0};
};
} // namespace Kvant

// Driver Implemenation
namespace Kvant {
template <typename DRIVABLE> void HoverDriver<DRIVABLE>::dolly(float d) {
  auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
  auto deltaPos = cam_dir * d;
  *_driven_pos += deltaPos;
  *_driven_tgt += deltaPos;
  retransform();
}

template <typename DRIVABLE> void HoverDriver<DRIVABLE>::truck(float d) {
  auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
  auto side_dir = glm::cross(cam_dir, _up);
  auto deltaPos = side_dir * d;
  *_driven_pos += deltaPos;
  *_driven_tgt += deltaPos;
  retransform();
}

template <typename DRIVABLE>
void HoverDriver<DRIVABLE>::follow(float a1, float a2) {
  auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
  cam_dir.y += -a2;
  auto side_dir = glm::cross(cam_dir, _up);
  cam_dir += side_dir * -a1;
  cam_dir = glm::normalize(cam_dir);
  *_driven_tgt = *_driven_pos + cam_dir;
  retransform();
}

template <typename DRIVABLE> void HoverDriver<DRIVABLE>::pan(float a1) {
  auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
  auto side_dir = glm::cross(cam_dir, _up);
  cam_dir += (side_dir * -a1);
  cam_dir = glm::normalize(cam_dir);
  *_driven_tgt = *_driven_pos + cam_dir;
  retransform();
}

template <typename DRIVABLE> void HoverDriver<DRIVABLE>::strafe(float d) {
  auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
  auto side_dir = glm::cross(cam_dir, _up);
  auto delta = side_dir * d;
  *_driven_pos += delta;
  *_driven_tgt += delta;
  retransform();
}

template <typename DRIVABLE> void DirectDriver<DRIVABLE>::update() {
  _transform = glm::translate(_transform, _translation);
  _transform *= _rotation;
  _transform = glm::scale(_transform, _scale);
  *_driven_transform = _transform;
}

template <typename DRIVABLE> void DirectDriver<DRIVABLE>::reset() {
  _translation = glm::vec3();
  _rotation = glm::mat4_cast(glm::quat(1, 0, 0, 0));
  _scale = glm::vec3(1, 1, 1);
  update();
}

template <typename DRIVABLE> void DirectDriver<DRIVABLE>::rotate(glm::vec3 v) {
  auto quat = glm::quat(v);
  _rotation = glm::mat4_cast(quat);
  update();
}

template <typename DRIVABLE> void DirectDriver<DRIVABLE>::rotate(glm::quat v) {
  _rotation = glm::mat4_cast(v);
  update();
}

template <typename DRIVABLE>
void DirectDriver<DRIVABLE>::look_at(glm::vec3 pos, glm::vec3 tgt,
                                     glm::vec3 up) {
  _rotation = glm::lookAt(pos, tgt, up);
  update();
}

template <typename DRIVABLE> void DirectDriver<DRIVABLE>::scale(glm::vec3 v) {
  _scale = v;
  update();
}

template <typename DRIVABLE>
void DirectDriver<DRIVABLE>::translate(glm::vec3 v) {
  _translation = v;
  update();
}
} // namespace Kvant

#endif // DRIVERS_HPP_INCLUDED