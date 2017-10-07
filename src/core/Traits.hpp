#ifndef TRAITS_HPP_INCLUDED
#define TRAITS_HPP_INCLUDED

#include <type_traits>

namespace Kvant {

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

    template <typename ACTUAL>
    class Asset : public IsEntity<ACTUAL>,
                  public IsDrivable<ACTUAL> {};
}

#endif // TRAITS_HPP_INCLUDED