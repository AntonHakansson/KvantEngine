#ifndef OpenglSurface_HPP_INCLUDED
#define OpenglSurface_HPP_INCLUDED

#include <glm/glm.hpp>

#include "utils/Checks.hpp"
#include "utils/Logger.hpp"

namespace Kvant::graphics::opengl {
    using namespace glm;

    template <typename GRAPHICS>
    struct OpenglSurface : blueprints::graphics::Surface<GRAPHICS, OpenglSurface<GRAPHICS>> {
        GLuint vertex_vbo;
        GLuint triangle_vbo;
        GLuint transform_vbo;
        GLuint world_vbo;

        OpenglSurface(const GraphicsContext<typename GRAPHICS::Backend>& ctx, Mesh* m)
            : Kvant::blueprints::graphics::Surface<GRAPHICS, OpenglSurface<GRAPHICS>>(ctx, m){
            int vertex_size = m->is_rigged ? 24 : 18;
            Kvant::Vertex test;
            this->calc_bounds(m);
            glGenBuffers(1, &vertex_vbo);
            glGenBuffers(1, &triangle_vbo);
            glGenBuffers(1, &transform_vbo);
            glGenBuffers(1, &world_vbo);

            this->n_vertices = m->vertices.size();
            this->n_triangles = m->triangles.size() / 3;

            const int vb_size = sizeof(GLfloat) * this->n_vertices * vertex_size;
            float* vb_data = (float*) malloc(vb_size);
            for (int i = 0; i < this->n_vertices; i++) {
                m->vertices[i].to_array( &vb_data[i*vertex_size] );

                if (m->is_rigged) {
                    std::runtime_error("Rigged not implemented");
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
            glBufferData(GL_ARRAY_BUFFER, vb_size, vb_data, GL_STATIC_DRAW);
            free(vb_data);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->triangle_vbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * this->n_triangles * 3,
                         &m->triangles[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        virtual ~OpenglSurface() {
            glDeleteBuffers(1, &vertex_vbo);
            glDeleteBuffers(1, &triangle_vbo);
            glDeleteBuffers(1, &transform_vbo);
            glDeleteBuffers(1, &world_vbo);
        }

        template <typename PIPELINE>
        void draw_instanced(const GRAPHICS& backend,
                            const PIPELINE& shader,
                            const glm::mat4* transform,
                            int count) const {
            GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, transform_vbo));
            shader.bind_instances();
            GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * count, transform, GL_DYNAMIC_DRAW));

            GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo));
            shader.enable_vertex_attributes();

            GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->triangle_vbo));
            GL_CHECK(glDrawElementsInstanced(GL_TRIANGLES, this->n_triangles * 3, GL_UNSIGNED_INT, (void*)0, count));

            shader.disable_all_vertex_attribs();
            // Imgui
            GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
            GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
            // Imgui
        }
    };
}

#endif // OpenglSurface_HPP_INCLUDED
