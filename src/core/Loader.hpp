#ifndef LOADER_HPP_INCLUDED
#define LOADER_HPP_INCLUDED

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "backends/blueprints/Graphics.hpp"
#include "core/Animation.hpp"
#include "core/Mesh.hpp"
#include "utils/lodepng.hpp"
#include "utils/Logger.hpp"

namespace Kvant {

    std::unique_ptr<Kvant::Model> read_obj(const char* filename);
    std::unique_ptr<Kvant::Model> read_smd(const char* filename);
    std::vector<Frame> read_animation(const char *filename);
    bool load_png_for_texture(unsigned char** img, unsigned int* width,
                              unsigned int* height, const char* filename);
}


#endif // LOADER_HPP_INCLUDED
