#ifndef EXTENSIONS_HPP_INCLUDED
#define EXTENSIONS_HPP_INCLUDED

#include <glm/gtc/matrix_transform.hpp>

#include "core/Traits.hpp"

namespace Kvant {

    namespace pos {
        static glm::mat4 origin(int) {
            return glm::mat4();
        }

        static std::function<glm::mat4(int)> look_at(glm::vec3 pos, glm::vec3 target) {
            return [pos, target](int) -> glm::mat4 {
                return glm::lookAt(pos, target, {0, 1, 0});
            };
        }
    }

    template <int C = 1>
    class Transforms : public IsTransformable<Transforms<C>> {
        public:
            template <typename CONTEXT>
            Transforms(const CONTEXT& ctx, std::function<glm::mat4(int)> pos_callback = [](int i){
                UNUSED(i);
                return glm::mat4(); // Return identity matrix by default
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
                return &_transforms[C];
            }

            operator std::array<glm::mat4, C>() const {
                return *_transforms;
            }
            
            const glm::mat4* get_transforms() const {
                return _transforms;
            }

            glm::mat4* get_transforms_ptr(int i=0) {
                return &_transforms[i];
            }

            void set_transforms(std::function<glm::mat4(int index)> f, int begin = 0, int end = C) {
                for (int j = begin; j < end; j++) {
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
    class Instances : public Transforms<C>,
                      public Container<T>,
                      public MayHaveDrawable<T, Instances<T, C>>,
                      public MayHaveDrivable<T, Instances<T, C>> {
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

    template <typename DRIVABLE>
    class no_driver
    {
      public:
        no_driver(const CoreContext &ctx, glm::mat4 *t, DRIVABLE *d)
        {
            UNUSED(ctx);
            UNUSED(t);
            UNUSED(d);
        }
    };

    /**
     * Kvant::driven can extend any Kvant::is_transformable by allocating one or more
     * Kvant::driver objects to manipulate one or more 3D transformation.
     *
     * @tparam T a valid gdt::is_transformable type.
     * @tparam DRIVER a driver type to instantiate for each transformation of T.
     */
    template <typename T, template <typename> typename DRIVER = no_driver>
    class Driven : public Container<T>,
                   public MayHaveDrawable<T, Driven<T, DRIVER>>,
                   public MayHaveDrivable<T, Driven<T, DRIVER>>,
                   public MayHaveTransformable<T, Driven<T, DRIVER>> {

        public:
            template <typename CONTEXT, typename... ARG>
            Driven(const CONTEXT& ctx,
                   const std::function<typename DRIVER<typename T::TDrivable>::initializer(int)>
                    &driver_callback = [](int i) -> typename DRIVER<typename T::TDrivable>::initializer {
                        UNUSED(i);
                        return typename DRIVER<typename T::TDrivable>::initializer();
                    },
                    const ARG&... args) : Container<T>(ctx, pos::origin, args...) {
                for (int j = 0; j < this->content()->size(); j++) {
                    _drivers.push_back(
                        std::make_unique<DRIVER<typename T::TDrivable>>(
                            ctx, this->get_transformable_ptr()->get_transforms_ptr(j),
                            this->get_drivable_ptr(), driver_callback(j)
                        )
                    );
                }
            }

            template <typename CONTEXT>
            void update(const CONTEXT& ctx) {
                for (int i = 0; i < this->content()->size(); i++) {
                    _drivers[i]->update();
                }
                this->content()->update(ctx);
            }

            const DRIVER<typename T::etype>& get_driver(int index = 0) const {
                return *_drivers[index].get();
            }
            DRIVER<typename T::etype>* get_driver_ptr(int index = 0) {
                return _drivers[index].get();
            }

            const T& drivable() const {
                return *this->ccontent();
            }

            T* drivable_ptr() {
                return this->content();
            }

            operator T() {
                return *this->content();
            }

        private:
            std::vector<std::unique_ptr<DRIVER<typename T::TDrivable>>> _drivers;

    };

}
#endif //EXTENSIONS_HPP_INCLUDED