#pragma once

#include <array>
#include <concepts>

namespace quxflux
{
  struct clamping_layout_index_calculator
  {
    template<typename Extents, std::size_t N>
      requires(std::signed_integral<typename Extents::index_type>)
    constexpr auto operator()(const Extents& extents, const std::array<typename Extents::index_type, N>& indices) const
    {
      using rank_type = typename Extents::index_type;

      std::array<typename Extents::index_type, N> r;

      for (size_t i = 0; i < N; ++i)
        r[i] = std::clamp(indices[i], rank_type{0}, extents.extent(i) - 1);

      return r;
    }
  };

  struct wrapping_layout_index_calculator
  {
    template<typename Extents, std::size_t N>
      requires(std::signed_integral<typename Extents::index_type>)
    constexpr auto operator()(const Extents& extents, const std::array<typename Extents::index_type, N>& indices) const
    {
      using rank_type = typename Extents::index_type;

      std::array<typename Extents::index_type, N> r;

      for (size_t i = 0; i < N; ++i)
      {
        const auto extent = extents.extent(i);
        const auto rem = indices[i] % extent;
        r[i] = rem < 0 ? extent + rem : rem;
      }

      return r;
    }
  };
}  // namespace quxflux