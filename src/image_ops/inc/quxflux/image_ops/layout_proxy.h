#pragma once

#include <quxflux/common/wrap_mdspan.h>

#include <utility>

namespace quxflux
{
  template<typename IndexCalculator, typename UnderlyingLayout = NS_MDSPAN::layout_right>
  struct layout_proxy
  {
    template<typename Extents>
    struct mapping
    {
      using extents_type = Extents;
      using index_type = typename extents_type::index_type;
      using size_type = typename extents_type::size_type;
      using rank_type = typename extents_type::rank_type;
      using layout_type = layout_proxy<IndexCalculator, UnderlyingLayout>;
      using underlying_mapping = UnderlyingLayout::template mapping<Extents>;

      template<typename OtherExtents, template<typename> typename OtherLayout>
      constexpr mapping(const OtherLayout<OtherExtents>& extents) noexcept : m_underlying_mapping(extents){};

      template<typename... Indices>
      constexpr index_type operator()(Indices... idxs) const noexcept
      {
        const auto converted_indices = IndexCalculator{}(m_underlying_mapping.extents(), std::array{idxs...});

        // pass the actual index calculation on to the IndexCalculator
        return [=]<size_t... Idx>(std::index_sequence<Idx...>)
        {
          // the underlying mapping will calculate the final 1d index
          return m_underlying_mapping(converted_indices[Idx]...);
        }
        (std::make_index_sequence<sizeof...(Indices)>());
      }

      constexpr const extents_type& extents() const noexcept { return m_underlying_mapping.extents(); }

      underlying_mapping m_underlying_mapping;
    };
  };

  template<typename NewLayout, typename ElementType, typename Extents, typename OldLayout, typename AccessorPolicy>
  constexpr auto change_layout(const NS_MDSPAN::mdspan<ElementType, Extents, OldLayout, AccessorPolicy> mdspan)
  {
    return NS_MDSPAN::mdspan<ElementType, Extents, NewLayout, AccessorPolicy>{mdspan};
  }
}  // namespace quxflux
