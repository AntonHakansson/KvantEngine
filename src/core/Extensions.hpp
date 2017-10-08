#ifndef EXTENSIONS_HPP_INCLUDED
#define EXTENSIONS_HPP_INCLUDED

#include "core/Traits.hpp"

namespace Kvant {

    namespace pos {
        static glm::mat4 origin(int) {
            return glm::mat4();
        }
    }

    template <int C = 1>
    class Transforms : public IsTransformable<Transforms<C>> {
        public:
            template <typename CONTEXT>
            Transforms(const CONTEXT& ctx, std::function<glm::mat4(int)> pos_callback = [](int i){
                UNUSED(i);
                return glm::mat4();
            })
            {
                ctx.graphics->create_instance_buffer(&_transforms, C);
                for (int j = 0; j < C; j++) {
                    _transforms[j] = pos_callback(j);
                }
            }

            unsigned int size() const {
                return C;
            }

            glm::mat4* begin() {
                return _transforms;
            }

            glm::mat4* end() {
                return _transforms[C];
            }

            operator std::array<glm::mat4, C>() const {
                return *_transforms;
            }
            
            const glm::mat4* get_transforms() const {
                return _transforms;
            }

            glm::mat4* get_transforms_ptr() const {
                return &_transforms;
            }

            void set_transforms(std::function<glm::mat4(int index)> f, int begin = 0, int end = C) {
                for (int j = 0; j < end; j++) {
                   _transforms[j] = f(j); 
                }
            }            

            template <typename CONTEXT>
            void update(const CONTEXT& ctx) {
                ctx.graphics->update_instance_buffer(_transforms, C);
            }

        protected:
            glm::mat4* _transforms;
    };

    template <typename T, int C = 1>
    class Instances : public Transforms<C>, public Container<T> {
        public:
            template <typename CONTEXT, typename... ARG>
            Instances(const CONTEXT& ctx, std::function<glm::mat4(int)> pos_callback = [](int i) {
                                                                                            UNUSED(i);
                                                                                            return glm::mat4();
                                                                                        }, const ARG &... a)
                    : Container<T>(ctx, a...), Transforms<C>(ctx, pos_callback) {}
            
            int size() const {
                return C;
            }
    };

    template <typename T>
    using Instance = Instances<T, 1>;
}
#endif //EXTENSIONS_HPP_INCLUDED