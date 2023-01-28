#pragma once

#include <quxflux/common/wrap_mdspan.h>

#include <concepts>

namespace quxflux
{
  template<typename Mdspan>
  concept mdspan_specialization =
    std::same_as<Mdspan, NS_MDSPAN::mdspan<typename Mdspan::element_type, typename Mdspan::extents_type,
                                           typename Mdspan::layout_type, typename Mdspan::accessor_type>>;
}
