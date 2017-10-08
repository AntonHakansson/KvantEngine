#ifndef SHADERS_HPP_INCLUDED
#define SHADERS_HPP_INCLUDED


namespace Kvant {

    template <typename GRAPHICS, typename S>
    class PipelineProxy {
        public:
            PipelineProxy(const S& s) : _s(s) { }
            const S& use(const GraphicsContext<GRAPHICS>& ctx) const {
                _s.use(ctx);
                return _s;
            }
        private:
            const S& _s;
    };

    template <typename GRAPHICS>
    struct Material {
        Material(const GraphicsContext<GRAPHICS>& ctx,
                 const typename GRAPHICS::Texture* d,
                 const typename GRAPHICS::Texture* n,
                 const typename GRAPHICS::Texture* s)
            : diffuse(d), normal(n), specular(s) { }
        
        template<typename SHADER>
        void bind(const GraphicsContext<GRAPHICS>& ctx, const SHADER& s) const {
            s.set_material(*this);
        }

        const typename GRAPHICS::Texture* diffuse;
        const typename GRAPHICS::Texture* normal;
        const typename GRAPHICS::Texture* specular;
    };

    
    template <typename GRAPHICS, typename T>
    class Pipeline : public GRAPHICS::BasePipeline {
        public:
            Pipeline(std::string resource_base_name) : GRAPHICS::BasePipeline(resource_base_name) { }
            
            PipelineProxy<GRAPHICS, T> by_proxy() {
                return PipelineProxy<GRAPHICS, T>(static_cast<const T&>(*this));
            }

            const T& use(const GraphicsContext<GRAPHICS>& ctx) const {
                GRAPHICS::BasePipeline::use(ctx);
                return static_cast<const T&>(*this);
            }
    };

    template<typename GRAPHICS>
    class ForwardPipeline : public Pipeline<GRAPHICS, ForwardPipeline<GRAPHICS>> {
        public:
            using Material = Kvant::Material<GRAPHICS>;
            ForwardPipeline(const GraphicsContext<GRAPHICS>& ctx)
                    : Pipeline<GRAPHICS, ForwardPipeline<GRAPHICS>>("res/shader/forward") {
                this->use(ctx);
                _tex = this->add_sampler("tex_diffuse");
                _ntex = this->add_sampler("tex_normal");
                _stex = this->add_sampler("tex_specular");
                _mvp = this->add_uniform("um4_mvp");
                _eye = this->add_uniform("uv3_eyepos");
                _vpos = this->add_attrib("av4_position");
                _vnor = this->add_attrib("av3_normal");
                _vtan = this->add_attrib("av3_tangent");
                _vuvs = this->add_attrib("av2_texcoord");
                _itfm = this->add_attrib("am4_transform");
            }

            template <typename CONTEXT, typename ENTITY>
            const ForwardPipeline& user_draw(const CONTEXT& ctx, const ENTITY& what) const {
                what.draw(ctx, *this);
                return *this;
            }

            void disable_all_vertex_attribs() const {
                this->disable_attrib(_vpos);
                this->disable_attrib(_vnor);
                this->disable_attrib(_vtan);
                this->disable_attrib(_vuvs);
                this->disable_attrib(_itfm);
                this->disable_attrib(_itfm + 1);
                this->disable_attrib(_itfm + 2);
                this->disable_attrib(_itfm + 3);
            }

            template <typename PLUGABLE>
            auto& operator<<(const PLUGABLE& cc) const {
                this->set_material(cc);
                return *this;
            }

            ForwardPipeline& set_material(const Material& solid_mat) const {
                this->bind_sampler(_tex, solid_mat.diffuse);
                this->bind_sampler(_tex, solid_mat.normal);
                this->bind_sampler(_tex, solid_mat.specular);
                return *this;
            }

            void bind_vertex_attribs() const {
                this->bind_float_attrib(_vpos,
                                        3,          // vec3
                                        18,         // total floats per vertex
                                        (void*)0);  // position in vertex
                
                this->bind_float_attrib(_vnor,
                                        3,                          // vec3
                                        18,                         // total floats per vertex
                                        (void*)(sizeof(float)*3));  // position in vertex
    
                this->bind_float_attrib(_vtan,
                                        3,                          // vec3
                                        18,                         // total floats per vertex
                                        (void*)(sizeof(float)*6));  // position in vertex

                this->bind_float_attrib(_vuvs,
                                        2,                          // vec2
                                        18,                         // total floats per vertex
                                        (void*)(sizeof(float)*12));  // position in vertex
           }

            const ForwardPipeline& set_modelview(glm::mat4 mvp) const {
                this->bind_uniform(_mvp, mvp);
                return *this;
            }

            const ForwardPipeline& set_eyepos(glm::vec3 eye) const {
                this->bind_uniform(_eye, eye);
                return *this;
            }

            template <typename CAMERA>
            const ForwardPipeline& set_camera(const CAMERA& c) const {
                // TODO: implement camera
                set_eyepos(c.entity().pos);
                set_modelview(c.entity().proj * c.get_transformable().get_transforms()[0]);
                return *this;
            }

            void enable_vertex_attributes() const {
                this->bind_vertex_attribs();
            }

        private:
            typename GRAPHICS::BasePipeline::Sampler _tex;
            typename GRAPHICS::BasePipeline::Sampler _ntex;
            typename GRAPHICS::BasePipeline::Sampler _stex;

            typename GRAPHICS::BasePipeline::Uniform _mvp;
            typename GRAPHICS::BasePipeline::Uniform _eye;

            typename GRAPHICS::BasePipeline::Attrib _vpos;
            typename GRAPHICS::BasePipeline::Attrib _vnor;
            typename GRAPHICS::BasePipeline::Attrib _vtan;
            typename GRAPHICS::BasePipeline::Attrib _vuvs;

            typename GRAPHICS::BasePipeline::Attrib _itfm;

    };

}

#endif // SHADERS_HPP_INCLUDED
