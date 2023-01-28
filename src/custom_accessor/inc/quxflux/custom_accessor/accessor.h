#pragma once

#include <quxflux/common/wrap_mdspan.h>

#include <quxflux/custom_accessor/restrict.h>

#include <bit>
#include <memory>
#include <tuple>
#include <type_traits>

namespace quxflux
{
  namespace detail
  {
    constexpr bool is_power_of_two(const std::size_t x)
    {
      return std::has_single_bit(x);
    }
  }  // namespace detail

  namespace access
  {
    struct restricted
    {};

    template<size_t Alignment>
      requires(detail::is_power_of_two(Alignment))
    struct aligned
    {};
  }  // namespace access

  namespace detail
  {
    template<typename Modifier>
    struct apply_modifier;

    template<>
    struct apply_modifier<std::identity>
    {
      [[nodiscard]] constexpr auto* operator()(auto* const ptr) { return ptr; }
    };

    template<>
    struct apply_modifier<access::restricted>
    {
      [[nodiscard]] constexpr auto* operator()(auto* const ptr) { return as_restrict(ptr); }
    };

    template<size_t Alignment>
    struct apply_modifier<access::aligned<Alignment>>
    {
      [[nodiscard]] constexpr auto* operator()(auto* const ptr) { return std::assume_aligned<Alignment>(ptr); }
    };

    template<typename Ptr, typename Modifier = std::identity, typename... Modifiers>
      requires(std::is_pointer_v<Ptr>)
    [[nodiscard]] constexpr auto* apply_modifiers(Ptr const ptr)
    {
      auto* const modified = apply_modifier<Modifier>{}(ptr);

      if constexpr (sizeof...(Modifiers) == 0)
        return modified;
      else
        return apply_modifiers<decltype(modified), Modifiers...>(modified);
    }
  }  // namespace detail

  template<typename T, typename... Modifiers>
  class accessor
  {
    using offset_policy = accessor;
    using element_type = T;
    using reference = T&;
    using pointer = T*;

    constexpr pointer offset(const pointer p, const size_t i) const noexcept { return p + i; }
    constexpr reference access(const pointer p, const size_t i) const noexcept
    {
      return *detail::apply_modifiers<pointer, Modifiers...>(offset(p, i));
    }
  };

  template<typename T, typename Extents, typename... Modifiers>
  constexpr auto make_mdspan_with_accessor(T* const ptr, const Extents& e)
  {
    return NS_MDSPAN::mdspan<T, Extents, NS_MDSPAN::layout_right, accessor<T, Modifiers...>>{ptr, e};
  }

}  // namespace quxflux
