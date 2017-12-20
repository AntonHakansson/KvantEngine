#ifndef MESH_HPP_INCLUDED
#define MESH_HPP_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/Logger.hpp"

namespace Kvant {
    using namespace glm;

    struct Vertex {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec3 binormal;
        vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
        vec2 uvs;

        static void vec2_to_array(vec2 b, float* out) {
            out[0] = b.x;
            out[1] = b.y;
        }
        
        static void vec3_to_array(vec3 b, float* out) {
            out[0] = b.x;
            out[1] = b.y;
            out[2] = b.z;
        }

        static void vec4_to_array(vec4 b, float* out) {
            out[0] = b.x;
            out[1] = b.y;
            out[2] = b.z;
            out[3] = b.w;
        }

        void to_array(float* p) {
            vec3_to_array(position, &p[0]);
            vec3_to_array(normal, &p[3]);
            vec3_to_array(tangent, &p[6]);
            vec3_to_array(binormal, &p[9]);
            vec2_to_array(uvs, &p[12]);
            vec4_to_array(color, &p[14]);
        }

    };

    static glm::vec3 triangle_tangent(Vertex vert1, Vertex vert2, Vertex vert3) {
        glm::vec3 pos1 = vert1.position;
        glm::vec3 pos2 = vert2.position;
        glm::vec3 pos3 = vert3.position;

        glm::vec2 uv1 = vert1.uvs;
        glm::vec2 uv2 = vert2.uvs;
        glm::vec2 uv3 = vert3.uvs;

        glm::vec3 comp_vec1 = pos2 - pos1;
        glm::vec3 comp_vec2 = pos3 - pos1;

        glm::vec2 uv_space1 = uv2 - uv1;
        glm::vec2 uv_space2 = uv3 - uv1;

        float r = 1.0f / ((uv_space1.x * uv_space2.y) - (uv_space2.x * uv_space1.y));

        glm::vec3 tdir = glm::vec3((uv_space1.x * comp_vec2.x - uv_space2.x * comp_vec1.x) * r,
                                    (uv_space1.x * comp_vec2.y - uv_space2.x * comp_vec1.y) * r,
                                    (uv_space1.x * comp_vec2.z - uv_space2.x * comp_vec1.z) * r);
    
        return glm::normalize(tdir);
    }

    static glm::vec3 triangle_binormal(Vertex vert1, Vertex vert2, Vertex vert3) {
        glm::vec3 pos1 = vert1.position;
        glm::vec3 pos2 = vert2.position;
        glm::vec3 pos3 = vert3.position;

        glm::vec2 uv1 = vert1.uvs;
        glm::vec2 uv2 = vert2.uvs;
        glm::vec2 uv3 = vert3.uvs;

        glm::vec3 comp_vec1 = pos2 - pos1;
        glm::vec3 comp_vec2 = pos3 - pos1;

        glm::vec2 uv_space1 = uv2 - uv1;
        glm::vec2 uv_space2 = uv3 - uv1;

        float r = 1.0f / ((uv_space1.x * uv_space2.y) - (uv_space2.x * uv_space1.y));

        glm::vec3 tdir = glm::vec3((uv_space1.y * comp_vec2.x - uv_space2.y * comp_vec1.x) * r,
                                    (uv_space1.y * comp_vec2.y - uv_space2.y * comp_vec1.y) * r,
                                    (uv_space1.y * comp_vec2.z - uv_space2.y * comp_vec1.z) * r);
    
        return glm::normalize(tdir);
    }

    static glm::vec3 triangle_normal(Vertex v1, Vertex v2, Vertex v3) {
        glm::vec3 edge1 = v2.position - v1.position;
        glm::vec3 edge2 = v3.position - v1.position;
        glm::vec3 normal = glm::cross(edge1, edge2);
        return glm::normalize(normal);
    }

    static float triangle_area(Vertex v1, Vertex v2, Vertex v3) {
        glm::vec3 ab = v2.position - v1.position;
        glm::vec3 ac = v3.position - v1.position;
        return 0.5 * glm::length(glm::cross(ab, ac));
    }

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

        void generate_tangents() {
            for (auto& v : vertices) {
                v.tangent = glm::vec3();
                v.binormal = glm::vec3();
            }

            int i = 0;
            while (i < triangles.size()) {
                // get indexes
                int t_i1 = this->triangles[i];
                int t_i2 = this->triangles[i + 1];
                int t_i3 = this->triangles[i + 2];

                Vertex v1 = this->vertices[t_i1];
                Vertex v2 = this->vertices[t_i2];
                Vertex v3 = this->vertices[t_i3];

                glm::vec3 face_tangent = triangle_tangent(v1, v2, v3);
                glm::vec3 face_binormal = triangle_binormal(v1, v2, v3);

                v1.tangent = face_tangent + v1.tangent;
                v2.tangent = face_tangent + v2.tangent;
                v3.tangent = face_tangent + v3.tangent;

                v1.binormal = face_binormal + v1.binormal;
                v2.binormal = face_binormal + v2.binormal;
                v3.binormal = face_binormal + v3.binormal;


                this->vertices[t_i1] = v1;
                this->vertices[t_i2] = v2;
                this->vertices[t_i3] = v3;

                i += 3;
            }

            for (auto& v : vertices) {
                v.tangent = glm::normalize(v.tangent);
                v.binormal = glm::normalize(v.binormal);
            }
        }
    };

    struct Model {
        std::vector<std::unique_ptr<Mesh>> meshes;

        void generate_tangents() {
            for (auto& m : meshes) m->generate_tangents();
        }
    };
}

namespace std {

    namespace _internal {
        static int rawcast(float x) {
            union {
                float f;
                int i;
            } u;
            u.f = x;
            return u.i;
        }

        static const int _hash(glm::vec2 v) {
            return abs(rawcast(v.x) ^ rawcast(v.y));
        }

        static const int _hash(glm::vec3 v) {
            return abs(rawcast(v.x) ^ rawcast(v.y) ^ rawcast(v.y));
        }
    }

    template <>
    struct hash<Kvant::Vertex> {
        std::size_t operator()(const Kvant::Vertex& v) const {
            using namespace _internal;
            std::size_t val = abs(_hash(v.position) ^ _hash(v.normal) ^ _hash(v.uvs));
            return val % 4096;
        }
    };
}

#endif // MESH_HPP_INCLUDED
