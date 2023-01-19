#include <image.h>
#include <layout_proxy.h>
#include <layout_index_calculators.h>
#include <ppm.h>

#include <iostream>
#include <random>

namespace
{
  using namespace quxflux;

  void fill(image<std::byte>& img)
  {
    const auto image_mdspan = img.as_mdspan();

    for (size_t r = 0; r < img.extents().height; ++r)
      for (size_t c = 0; c < img.extents().width; ++c)
        image_mdspan(r, c) = std::byte{static_cast<uint8_t>(r * img.extents().width + c)};
  }

  template<typename Mdspan>
  void apply_box_filter(const Mdspan mdspan, const ptrdiff_t filter_size)
  {
    for (ptrdiff_t r = 0; r < mdspan.extent(0); ++r)
      for (ptrdiff_t c = 0; c < mdspan.extent(1); ++c)
      {
        float buf{};

        for (ptrdiff_t delta_r = -filter_size / 2; delta_r <= filter_size / 2; ++delta_r)
          for (ptrdiff_t delta_c = -filter_size / 2; delta_c <= filter_size / 2; ++delta_c)
            buf += static_cast<float>(mdspan(r + delta_r, c + delta_c));

        buf /= static_cast<float>(filter_size * filter_size);
        mdspan(r, c) = static_cast<typename Mdspan::element_type>(buf);
      }
  }
}  // namespace

int main(int, char**)
{
  image<std::byte> image({128, 128});
  fill(image);

  export_grayscale_ppm(image, "input.ppm");

  apply_box_filter(change_layout<layout_proxy<clamping_layout_index_calculator>>(image.as_mdspan()), 5);
  export_grayscale_ppm(image, "box_filtered_clamp.ppm");

  apply_box_filter(change_layout<layout_proxy<wrapping_layout_index_calculator>>(image.as_mdspan()), 5);
  export_grayscale_ppm(image, "box_filtered_wrap.ppm");

  return 0;
}
