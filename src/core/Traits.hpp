#ifndef TRAITS_HPP_INCLUDED
#define TRAITS_HPP_INCLUDED

#include <type_traits>

namespace Kvant {

// Thanks to David Rodríguez-dribeas for http://stackoverflow.com/a/11814074
template <bool Condition, typename Then, typename Else = void> struct if_ {
  typedef Then type;
};
template <typename Then, typename Else> struct if_<false, Then, Else> {
  typedef Else type;
};

template <typename CONTENT> struct Container {
  using etype = typename CONTENT::etype;
  CONTENT _content;

  CONTENT *content() { return &_content; }
  const CONTENT *ccontent() const { return &_content; }

  template <typename CONTEXT, typename... ZZ>
  Container(const CONTEXT &ctx, ZZ &... rest) : _content(ctx, rest...) {}

  const etype &entity() const {
    return static_cast<const etype &>(_content.entity());
  }

  etype *entity_ptr() { return static_cast<etype *>(_content.entity_ptr()); }

  operator etype() const {
    return *static_cast<etype *>(_content.entity_ptr());
  }
};

template <typename ENTITY> struct IsEntity {
  using etype = ENTITY;
  const etype &entity() const { return static_cast<const etype &>(*this); }

  etype *entity_ptr() { return static_cast<etype *>(this); }

  operator etype() const { return *static_cast<etype *>(*this); }
};

template <typename DRAWABLE> struct IsDrawable {
  using TDrawable = DRAWABLE;
  const TDrawable &get_drawable() const {
    return static_cast<const DRAWABLE &>(*this);
  }

  TDrawable *get_drawable_ptr() { return static_cast<DRAWABLE *>(this); }
};

template <typename DRAWABLE, typename ME, typename Enable = void>
struct MayHaveDrawable {};

template <typename DRAWABLE, typename ME>
struct MayHaveDrawable<
    DRAWABLE, ME, typename if_<false, typename DRAWABLE::TDrawable>::type> {
  using TDrawable = typename DRAWABLE::TDrawable;
  const TDrawable &get_drawable() const {
    return static_cast<const ME *>(this)->ccontent()->get_drawable();
  }

  TDrawable *get_drawable_ptr() {
    return static_cast<ME *>(this)->content()->get_drawable_ptr();
  }
};

template <typename DRIVABLE> struct IsDrivable {
  using TDrivable = DRIVABLE;
  const TDrivable *get_drivable() const {
    return static_cast<const DRIVABLE *>(*this);
  }

  TDrivable *get_drivable_ptr() { return static_cast<DRIVABLE *>(this); }
};

template <typename DRIVABLE, typename ME, typename Enable = void>
struct MayHaveDrivable {};

template <typename DRIVABLE, typename ME>
struct MayHaveDrivable<
    DRIVABLE, ME, typename if_<false, typename DRIVABLE::TDrivable>::type> {

  using TDrivable = typename DRIVABLE::TDrivable;
  const TDrivable *get_drivable() const {
    return static_cast<const ME *>(this)->ccontent()->get_drivable();
  }

  TDrivable *get_drivable_ptr() {
    return static_cast<ME *>(this)->content()->get_drivable_ptr();
  }
};

template <typename ME> struct IsTransformable {
  using TTransformable = ME;
  const TTransformable &get_transformable() const {
    return static_cast<const ME &>(*this);
  };

  TTransformable *get_transformable_ptr() { return static_cast<ME *>(this); }
};

template <typename TRANSFORMABLE, typename ME, typename Enable = void>
struct MayHaveTransformable {};

template <typename TRANSFORMABLE, typename ME>
struct MayHaveTransformable<
    TRANSFORMABLE, ME,
    typename if_<false, typename TRANSFORMABLE::TTransformable>::type> {

  using TTransformable = typename TRANSFORMABLE::TTransformable;
  const TTransformable &get_transformable() const {
    return static_cast<const ME *>(this)->ccontent()->get_transformable();
  }

  TTransformable *get_transformable_ptr() {
    return static_cast<ME *>(this)->content()->get_transformable_ptr();
  }
};

template <typename ACTUAL>
class Asset : public IsEntity<ACTUAL>, public IsDrivable<ACTUAL> {};
} // namespace Kvant

#endif // TRAITS_HPP_INCLUDED