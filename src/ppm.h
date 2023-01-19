#pragma once

#include <image.h>

#include <filesystem>
#include <fstream>
#include <cstddef>

namespace quxflux
{
  inline void export_grayscale_ppm(const image<std::byte>& image, const std::filesystem::path& path)
  {
    std::ofstream ofs(path, std::ios_base::binary);

    if (!ofs)
      throw std::runtime_error("Cannot open file at path " + path.string() + " for writing");

    const auto [width, height] = image.extents();
    constexpr auto max_val = std::numeric_limits<std::uint8_t>::max();

    ofs << "P5" << '\n';
    ofs << width << '\n';
    ofs << height << '\n';
    ofs << static_cast<std::uint32_t>(max_val) << '\n';

    for (int i = 0; i < height; ++i)
      ofs.write(reinterpret_cast<const char*>(image.row_ptr(i)), width);
  }
}  // namespace quxflux