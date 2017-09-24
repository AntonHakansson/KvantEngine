#ifndef MESH_HPP_INCLUDED
#define MESH_HPP_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Kvant {
    using namespace glm;

    struct Vertex {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec3 binormal;
        vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
        vec2 uvs;

        void to_array(float* p) {
            p[0] = *glm::value_ptr(position);
            p[3] = *glm::value_ptr(normal);
            p[6] = *glm::value_ptr(tangent);
            p[9] = *glm::value_ptr(binormal);
            p[12] = *glm::value_ptr(uvs);
            p[14] = *glm::value_ptr(color);
        }

    };

    inline bool operator==(const Vertex& v1, const Vertex& v2) {
        if (v1.position != v2.position)
            return false;
        if (v1.normal != v2.normal)
            return false;
        if (v1.uvs != v2.uvs)
            return false;

        return true;
    }

    struct Vertex_weights {
        int bone_ids[3] = {0, 0, 0};
        float bone_weights[3] = {0, 0, 0};
    };

    struct Mesh {

        std::vector<Vertex> vertices;
        std::vector<uint32_t> triangles;
        bool is_rigged = false;
        std::vector<Vertex_weights> weights;



    };
}

#endif // MESH_HPP_INCLUDED