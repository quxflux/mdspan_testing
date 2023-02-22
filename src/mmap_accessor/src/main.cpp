#include <quxflux/common/wrap_mdspan.h>

#include <quxflux/mmap_accessor/mmap_accessor.h>

#include <write_mdspan.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <vector>

namespace
{
  namespace qf = quxflux;

  template<typename T>
  auto to_mdspan(const std::span<T> values, const ptrdiff_t n_rows, const ptrdiff_t n_cols)
  {
    return NS_MDSPAN::mdspan{values.data(), NS_MDSPAN::dextents<ptrdiff_t, 2>{n_rows, n_cols}};
  }

  void print(const qf::mdspan_specialization auto& mdspan)
  {
    for (size_t r = 0; r < mdspan.extent(0); ++r)
    {
      std::cout << "[ ";
      for (size_t c = 0; c < mdspan.extent(1); ++c)
      {
        std::cout << std::setw(3) << mdspan(r, c);

        if (c + 1 < mdspan.extent(1))
          std::cout << ", ";
      }

      std::cout << " ]\n";
    }
  }
}  // namespace

int main()
{
  {
    constexpr ptrdiff_t n_rows = 32;
    constexpr ptrdiff_t n_cols = 16;
    std::vector<int32_t> values(n_rows * n_cols);

    std::ranges::generate(values, [i = 0]() mutable { return i++; });
    qf::write("int32_matrix.bin", to_mdspan(std::span{values}, n_rows, n_cols));
  }

  print(qf::create_memmapped_mdspan2d<const int32_t>("int32_matrix.bin"));

  return EXIT_SUCCESS;
}