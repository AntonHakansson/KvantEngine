#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
  Camera(const CONTEXT &ctx, Screen *screen, float fov = 0.48f,
         float near = 0.1f, float far = 1024.0f)
      : _fov(fov), _near(near), _far(far) {
    this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
    this->proj = glm::mat4();
    this->tgt = glm::vec3();
    screen->subscribe(this);
  }

  void imgui() {
    if (ImGui::CollapsingHeader("camera")) {
      ImGui::PushItemWidth(100);
      ImGui::SliderFloat("fov", &_fov, 0.01f, 1.0f);
      ImGui::SameLine();
      ImGui::SliderFloat("near", &_near, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::SliderFloat("far", &_far, 0.0f, 3000.0f);
      ImGui::PopItemWidth();
      ImGui::SliderFloat3("pos", glm::value_ptr(pos), -100.0f, 100.0f);
      ImGui::SliderFloat3("tgt", glm::value_ptr(tgt), -100.0f, 100.0f);
      updateProjection();
    }
  }

protected:
  float _fov, _near, _far;
  unsigned int _width, _height;

private:
  void on_screen_resize(unsigned int width, unsigned int height) {
    _width = width;
    _height = height;
    updateProjection();
  }

  void updateProjection() {
    proj = glm::perspective(_fov, (float)_height / (float)_width, _near, _far);
  }
};
} // namespace Kvant

#endif // CAMERA_HPP_INCLUDED