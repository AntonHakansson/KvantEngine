#include "core/Loader.hpp"

namespace Kvant {

    Animation::Animation(std::string filename, const Skeleton& s, bool loop) {
        // TODO Load animation
        // _frames = Kvant::read_animation(filename.c_str());
        _skeleton = s;
        _loop = loop;
    }
}