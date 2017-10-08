#ifndef TRAITS_HPP_INCLUDED
#define TRAITS_HPP_INCLUDED

#include <type_traits>

namespace Kvant {

    template <typename CONTENT>
    struct Container {
        using etype = typename CONTENT::etype;
        CONTENT _content;

        CONTENT* content() { return &_content; }
        const CONTENT* ccontent() const { return &_content; }

        template <typename CONTEXT, typename... ZZ>
        Container(const CONTEXT& ctx, ZZ&... rest) : _content(ctx, rest...) {
        }

        const etype& entity() const {
            return static_cast<const etype&>(_content.entity());
        }

        etype* entity_ptr() {
            return static_cast<etype*>(_content.entity_ptr());
        }

        operator etype() const {
            return *static_cast<etype*>(_content.entity_ptr());
        }
    };

    template <typename ENTITY>
    struct IsEntity {
        using etype = ENTITY;
        const etype& entity() const {
            return static_cast<const etype&>(*this);
        }

        etype* entity_ptr() {
            return static_cast<etype*>(*this);
        }

        operator etype() const {
            return *static_cast<etype*>(*this);
        }
    };

    template <typename DRAWABLE>
    struct IsDrawable {
        using TDrawable = DRAWABLE;
        const TDrawable& get_drawable() const {
            return static_cast<const DRAWABLE&>(*this);
        }

        TDrawable* get_drawable_ptr() {
            return static_cast<TDrawable*>(this);
        }
    };

    template <typename DRIVABLE>
    struct IsDrivable {
        using TDrivable = DRIVABLE;
        const TDrivable* get_drivable() const {
            return static_cast<const TDrivable*>(*this);
        }

        TDrivable* get_drivable_ptr() {
            return static_cast<TDrivable*>(*this);
        }
    };

    template <typename TRANSFORMABLE>
    struct IsTransformable {
        using TTransformable = TRANSFORMABLE;
        const TTransformable& get_transformable() const {
            return static_cast<const TTransformable>(*this);
        };

        TTransformable* get_transformable_ptr() {
            return static_cast<TTransformable*>(*this);
        }
    };

    template <typename ACTUAL>
    class Asset : public IsEntity<ACTUAL>,
                  public IsDrivable<ACTUAL> {};
}

#endif // TRAITS_HPP_INCLUDED