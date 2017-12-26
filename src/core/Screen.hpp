#ifndef SCREEN_HPP_INCLUDED
#define SCREEN_HPP_INCLUDED

#include <vector>

namespace Kvant {

struct Screen {
  int w, h;
  struct Subscriber {
    virtual void on_screen_resize(unsigned int width, unsigned int height) = 0;
  };

  void notify_Subscribers() {
    for (auto s : _subsribers) {
      s->on_screen_resize(w, h);
    }
  }

  void subscribe(Subscriber *s) {
    _subsribers.push_back(s);
    s->on_screen_resize(w, h);
  }

  std::vector<Subscriber *> _subsribers;
};

} // namespace Kvant

#endif // SCREEN_HPP_INCLUDED