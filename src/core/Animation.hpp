#ifndef ANIMATION_HPP_INCLUDED
#define ANIMATION_HPP_INCLUDED

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Kvant {

struct frame {
    std::vector<int> bone_parents;
    std::vector<glm::vec3> bone_position;
    std::vector<glm::quat> bone_rotations;
    std::vector<glm::mat4> bone_transforms;
    std::vector<glm::vec4> bone_inv_transforms;
};

struct bone {
    std::string name;
    int parent;

    bone(std::string n, int p) : name(n), parent(p) {}
};

struct skeleton {
    std::vector<bone> bones;
    frame rest;

    int n_bones() const {
        return bones.size();
    }
};

}

#endif // ANIMATION_HPP_INCLUDED
