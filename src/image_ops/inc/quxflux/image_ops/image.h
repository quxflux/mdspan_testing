#pragma once

#include <quxflux/common/wrap_mdspan.h>

#include <cstddef>
#include <vector>

namespace quxflux
{
  struct extents_t
  {
    ptrdiff_t width = 0;
    ptrdiff_t height = 0;
  };

  template<typename Pixel>
  class image
  {
  public:
    constexpr image() = default;

    image(const extents_t& extents, std::vector<Pixel> data) : m_extents(extents), m_storage(std::move(data)) {}
    image(const extents_t& extents) : image(extents, std::vector<Pixel>(extents.width * extents.height)) {}

    constexpr extents_t extents() const { return m_extents; }

    constexpr auto as_mdspan() const { return as_mdspan_impl(*this); }
    constexpr auto as_mdspan() { return as_mdspan_impl(*this); }

  private:
    static constexpr auto as_mdspan_impl(auto& image)
    {
      return NS_MDSPAN::mdspan(image.m_storage.data(),
                               NS_MDSPAN::extents<ptrdiff_t, std::dynamic_extent, std::dynamic_extent>{
                                 image.m_extents.height, image.m_extents.width});
    }

    extents_t m_extents;
    std::vector<Pixel> m_storage;
  };
}  // namespace quxflux
