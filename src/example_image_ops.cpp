#include <image.h>
#include <layout_proxy.h>
#include <layout_index_calculators.h>
#include <ppm.h>

#include <stdexcept>

namespace
{
  using namespace quxflux;

  template<typename Mdspan>
  void apply_box_filter(const Mdspan mdspan, const ptrdiff_t filter_size)
  {
    if (filter_size % 2 != 1 || filter_size < 3)
      throw std::invalid_argument("filter size must be odd and greater than or equal to 3");

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
  const auto image = import_ppm(R"(reporter-camera.ppm)");

  {
    auto img_copy = image;
    apply_box_filter(change_layout<layout_proxy<clamping_layout_index_calculator>>(img_copy.as_mdspan()), 15);
    export_ppm(img_copy, "box_filtered_clamp.ppm");
  }

  {
    auto img_copy = image;
    apply_box_filter(change_layout<layout_proxy<wrapping_layout_index_calculator>>(img_copy.as_mdspan()), 15);
    export_ppm(img_copy, "box_filtered_wrap.ppm");
  }

  {
    auto img_copy = image;
    apply_box_filter(change_layout<layout_proxy<reflecting_layout_index_calculator>>(img_copy.as_mdspan()), 15);
    export_ppm(img_copy, "box_filtered_reflect.ppm");
  }

  return 0;
}
