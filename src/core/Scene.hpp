#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include "backends/blueprints/Platform.hpp"
#include "core/Screen.hpp"

namespace Kvant {
template <typename CONTEXT> class Scene {
public:
  Scene() {}
  virtual ~Scene() {}

  virtual void update(const CONTEXT &ctx) = 0;

  virtual void render(const CONTEXT &ctx) = 0;

  virtual void update_scenes(const CONTEXT &ctx) = 0;

  // These gets called if a scene gets pushed/popped from scene stack
  virtual void resume(const CONTEXT &ctx) = 0;
  virtual void pause(const CONTEXT &ctx) = 0;

  virtual void imgui(const CONTEXT &ctx){};

  virtual void on_screen_resize(const CONTEXT &ctx) { render(ctx); }

private:
};

template <typename CONTEXT> class empty_scene : public Scene<CONTEXT> {
public:
  empty_scene(const CONTEXT &ctx, Kvant::Screen *screen) {}
  virtual ~empty_scene() {}
  void update(const CONTEXT &ctx) override {
    if (ctx.get_platform()->is_key_pressed(key::ESC)) {
      ctx.quit();
    }
  }
  void render(const CONTEXT &ctx) override {}
};

template <typename CONTEXT> class SceneManager {
  using TScene = Scene<CONTEXT>;

public:
  SceneManager() {}

  TScene *get_scene() { return _scenes.back().get(); }

  template <typename NEW_SCENE> void update_scenes(const CONTEXT &ctx) {
    if (!_scenes.empty()) {
      _scenes.pop_back();
    }
    _scenes.emplace_back(std::make_unique<NEW_SCENE>(ctx));
  }

  template <typename NEW_SCENE> void push_scene(const CONTEXT &ctx) {
    if (!_scenes.empty()) {
      _scenes.back().get()->pause(ctx);
    }
    _scenes.emplace_back(std::make_unique<NEW_SCENE>(ctx));
    ctx.get_platform()->release_mouse();
  }

  void pop_scene(const CONTEXT &ctx) {
    if (_scenes.empty())
      return;
    _scenes.pop_back();
    if (_scenes.empty())
      return;
    _scenes.back().get()->resume(ctx);
    ctx.get_platform()->release_mouse();
  }

private:
  std::vector<std::unique_ptr<TScene>> _scenes;
};
} // namespace Kvant

#endif // SCENE_HPP_INCLUDED