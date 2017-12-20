#ifndef ANIMATION_HPP_INCLUDED
#define ANIMATION_HPP_INCLUDED

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Kvant {

    struct Frame {
        std::vector<int> bone_parents;
        std::vector<glm::vec3> bone_positions;
        std::vector<glm::quat> bone_rotations;
        std::vector<glm::mat4> bone_transforms;
        std::vector<glm::vec4> bone_inv_transforms;
    };

    struct Bone {
        std::string name;
        int parent;

        Bone(std::string n, int p) : name(n), parent(p) {}
    };

    struct Skeleton {
        std::vector<Bone> bones;
        Frame rest;

        int n_bones() const {
            return bones.size();
        }
    };

}

#endif // ANIMATION_HPP_INCLUDED
