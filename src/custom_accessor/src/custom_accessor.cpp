#include <quxflux/common/concepts.h>
#include <quxflux/common/wrap_mdspan.h>

#include <quxflux/custom_accessor/accessor.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <span>
#include <vector>

namespace
{
  namespace qf = quxflux;

  using duration_t = std::chrono::duration<float, std::milli>;

  constexpr void cwise_product(const qf::mdspan_specialization auto& mdspan_out,
                               const qf::mdspan_specialization auto& mdspan_a,
                               const qf::mdspan_specialization auto& mdspan_b)
  {
    assert(mdspan_out.extents() == mdspan_a.extents() && mdspan_a.extents() == mdspan_b.extents() &&
           "extents of all spans must match");

    const auto rows = mdspan_a.extents().extent(0);
    const auto cols = mdspan_a.extents().extent(1);

    for (size_t i = 0; i < rows; ++i)
      for (size_t k = 0; k < cols; ++k)
        mdspan_out(i, k) = mdspan_a(i, k) * mdspan_b(i, k);
  }

  template<typename T, typename... Modifiers>
  auto profile_single(const std::span<T> out, const std::span<T> a, const std::span<T> b, const ptrdiff_t rows,
                      const ptrdiff_t cols)
  {
    if (const auto n_coeffs = rows * cols; std::ranges::ssize(out) != n_coeffs || std::ranges::ssize(a) != n_coeffs ||
                                           std::ranges::ssize(b) != n_coeffs) [[unlikely]]
      throw std::invalid_argument("size of spans must equal number of coefficients");

    using clock = std::chrono::high_resolution_clock;

    using extents_t = NS_MDSPAN::extents<ptrdiff_t, std::dynamic_extent, std::dynamic_extent>;
    const auto extents = extents_t{rows, cols};

    const auto start = clock::now();

    if constexpr (sizeof...(Modifiers) == 0)
    {
      cwise_product(NS_MDSPAN::mdspan{out.data(), extents}, NS_MDSPAN::mdspan{a.data(), extents},
                    NS_MDSPAN::mdspan{b.data(), extents});
    } else
    {
      cwise_product(qf::make_mdspan_with_accessor<T, extents_t, Modifiers...>(out.data(), extents),
                    qf::make_mdspan_with_accessor<T, extents_t, Modifiers...>(a.data(), extents),
                    qf::make_mdspan_with_accessor<T, extents_t, Modifiers...>(b.data(), extents));
    }

    return std::chrono::duration_cast<duration_t>(clock::now() - start);
  }

  template<typename T>
  void prevent_optimization(T t)
  {
    static volatile T out;
    out = t;
  }

  void fill_random(const std::span<float> rng, auto& rd)
  {
    std::ranges::generate(rng, [&, dis = std::uniform_real_distribution<float>{}]() mutable { return dis(rd); });
  }

  template<typename T, typename... Modifiers>
  auto profile(const std::span<T> out, const std::span<T> a, const std::span<T> b, const ptrdiff_t rows,
               const ptrdiff_t cols, auto& rd)
  {
    std::array<duration_t, 100> timings{};
    for (size_t i = 0; i < timings.size(); ++i)
    {
      fill_random(a, rd);
      fill_random(b, rd);

      timings[i] = profile_single(out, a, b, rows, cols);
      prevent_optimization(std::accumulate(out.begin(), out.end(), T{}));
    }

    std::ranges::nth_element(timings, std::next(std::ranges::begin(timings), std::ranges::ssize(timings) / 2));
    return *std::next(std::ranges::begin(timings), std::ranges::ssize(timings) / 2);
  }
}  // namespace

int main(int, char**)
{
  static constexpr ptrdiff_t n_rows = 4096;
  static constexpr ptrdiff_t n_cols = 2048;

  std::vector<float> r(n_rows * n_cols);
  std::vector<float> a = r;
  std::vector<float> b = r;

  {
    std::mt19937 rd{42};
    std::cout << "default mdspan cwise product took " << profile<float>(r, a, b, n_rows, n_cols, rd) << "\n";
  }
  {
    std::mt19937 rd{42};
    std::cout << "mdspan<access::restricted> cwise product took "
              << profile<float, qf::access::restricted>(r, a, b, n_rows, n_cols, rd) << "\n";
  }
  {
    std::mt19937 rd{42};
    std::cout << "mdspan<access::aligned<64>> cwise product took "
              << profile<float, qf::access::aligned<64>>(r, a, b, n_rows, n_cols, rd) << "\n";
  }
  {
    std::mt19937 rd{42};
    std::cout << "mdspan<access::restricted, access::aligned<64>> cwise product took "
              << profile<float, qf::access::restricted, qf::access::aligned<64>>(r, a, b, n_rows, n_cols, rd) << "\n";
  }

  return EXIT_SUCCESS;
}
