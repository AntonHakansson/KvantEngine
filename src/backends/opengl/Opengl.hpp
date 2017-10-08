#ifndef OPENGL_HPP_INCLUDED
#define OPENGL_HPP_INCLUDED

#include <GLES3/gl31.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#include "backends/blueprints/Graphics.hpp"
#include "utils/Checks.hpp"
#include "core/Mesh.hpp"

#include "backends/opengl/OpenglBuffer.hpp"
#include "backends/opengl/OpenglShader.hpp"
#include "backends/opengl/OpenglSurface.hpp"

namespace Kvant::graphics::opengl {
    template <typename PLATFORM>
    class Backend : public blueprints::graphics::Backend<Backend<PLATFORM>>, Screen::Subscriber {
        public:
            using Pipeline = OpenglPipeline<Backend, PLATFORM>;
            using BasePipeline = OpenglPipeline<Backend, PLATFORM>;
            using CBackend = Backend<PLATFORM>;
            using Surface = OpenglSurface<CBackend>;
            using Texture = OpenglTexture<CBackend>;

            static const OpenglRenderPassClearCmd clear;
            static const OpenglScreenBuffer<Backend> screen_buffer;
            
            Backend(PLATFORM* p, Screen* screen) {
                screen->subscribe(this);
            }

            virtual ~Backend() {
                for (auto* p : _instance_bufs) {
                    delete[] p;
                }
            }

            void clear_screen() const override {
                GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
            }

            void blend_on() const {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            void blend_off() const {
                glDisable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            void cull_on() const {
                glEnable(GL_CULL_FACE);
            }

            void cull_off() const {
                glDisable(GL_CULL_FACE);
            }

            void on_screen_resize(unsigned int w, unsigned int h) override {
                glViewport(0, 0, w, h);
            }

            void update_frame() override {

            }

            template <typename... SHADER>
            void process_cmds(std::function<void(const SHADER &...)> cmds, const SHADER &... s) {
                cmds(s...);
            }

            void render_quad() {
                if (quadVAO == 0) {
                    GLfloat quad_vertices[] = {
                        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                    };
                    glGenVertexArrays(1, &quadVAO);
                    glGenBuffers(1, &quadVBO);
                    glBindVertexArray(quadVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
                }
                glBindVertexArray(quadVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
            }

            glm::mat4* create_instance_buffer(glm::mat4** d, int c) {
                *d = new glm::mat4[c];
                _instance_bufs.push_back(*d);
            }

            void update_instance_buffer(glm::mat4* data, int c) {
                
            }

            bool created = false;
            GLuint quadVAO = 0;
            GLuint quadVBO;            

        private:
            std::vector<glm::mat4*> _instance_bufs;
    };

    template <typename PLATFORM>
    const OpenglRenderPassClearCmd Backend<PLATFORM>::clear;

    template <typename PLATFORM>
    const OpenglScreenBuffer<Backend<PLATFORM>> Backend<PLATFORM>::screen_buffer;
}


#endif // OPENGL_HPP_INCLUDED
