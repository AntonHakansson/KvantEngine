#ifndef LOADER_HPP_INCLUDED
#define LOADER_HPP_INCLUDED

#include <algorithm>
#include <fstream>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

/* assimp include files. These three are usually needed. */
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "backends/blueprints/Graphics.hpp"
#include "core/Animation.hpp"
#include "core/Shaders.hpp"
#include "utils/Logger.hpp"
#include "utils/lodepng.hpp"

namespace Kvant {

struct RawModelData {
  Model model;
  std::vector<std::string> diffuses;
  std::vector<std::string> normals;
  std::vector<std::string> speculars;
};

RawModelData read_model(const char *filename);
bool load_png_for_texture(unsigned char **img, unsigned int *width,
                          unsigned int *height, const char *filename);
} // namespace Kvant

#endif // LOADER_HPP_INCLUDED
