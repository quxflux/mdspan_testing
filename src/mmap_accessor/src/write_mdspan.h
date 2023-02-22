#pragma once

#include <quxflux/common/concepts.h>
#include <quxflux/common/wrap_mdspan.h>

#include <quxflux/mmap_accessor/mmap_accessor.h>

#include <filesystem>
#include <fstream>
#include <span>

namespace quxflux
{
  template<mdspan_specialization Mdspan>
    requires(Mdspan::extents_type::rank() == 2 && std::same_as<typename Mdspan::layout_type, NS_MDSPAN::layout_right>)
  void write(const std::filesystem::path& path, const Mdspan& mdspan)
  {
    std::ofstream ofs(path.string(), std::ios_base::binary);
    ofs.exceptions(std::ifstream::failbit);

    using T = typename Mdspan::value_type;

    const uint64_t n_rows = static_cast<uint64_t>(mdspan.extents().extent(0));
    const uint64_t n_cols = static_cast<uint64_t>(mdspan.extents().extent(1));
    const std::span<const T> span{mdspan.data_handle(), n_rows * n_cols};

    ofs << identifier_for_type_info(typeid(T));
    ofs.write(reinterpret_cast<const char*>(&n_rows), sizeof(n_rows));
    ofs.write(reinterpret_cast<const char*>(&n_cols), sizeof(n_cols));
    ofs.write(reinterpret_cast<const char*>(span.data()), sizeof(T) * span.size());
  }
}  // namespace quxflux
