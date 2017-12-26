#ifndef LOADER_HPP_INCLUDED
#define LOADER_HPP_INCLUDED

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

/* assimp include files. These three are usually needed. */
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "backends/blueprints/Graphics.hpp"
#include "core/Animation.hpp"
#include "core/Shaders.hpp"
#include "utils/lodepng.hpp"
#include "utils/Logger.hpp"

namespace Kvant {

    struct RawModelData {
        Model model;
        std::vector<std::string> diffuses;
        std::vector<std::string> normals;
        std::vector<std::string> speculars;
    };

    RawModelData read_model(const char* filename);
    bool load_png_for_texture(unsigned char** img, unsigned int* width,
                              unsigned int* height, const char* filename);
}


#endif // LOADER_HPP_INCLUDED
