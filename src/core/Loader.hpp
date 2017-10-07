#ifndef LOADER_HPP_INCLUDED
#define LOADER_HPP_INCLUDED

#include <memory>
#include "core/Mesh.hpp"
#include "core/Animation.hpp"

namespace Kvant {

    std::unique_ptr<Kvant::Model> read_smd(const char* filename);
    bool load_png_for_texture(unsigned char** img, unsigned int* width,
                              unsigned int* height, const char* filename);
}


#endif // LOADER_HPP_INCLUDED