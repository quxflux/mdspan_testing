#pragma once

#include <quxflux/image_ops/image.h>

#include <cstddef>
#include <filesystem>
#include <fstream>

namespace quxflux
{
  [[nodiscard]] inline image<std::byte> import_ppm(const std::filesystem::path& path)
  {
    std::ifstream ifs(path, std::ios_base::binary);

    if (!ifs)
      throw std::runtime_error("Cannot open file at path " + path.string());

    ifs.exceptions(std::ifstream::failbit);

    std::string ppm_marker;
    ifs >> ppm_marker;

    if (ppm_marker != "P5")
      throw std::runtime_error("Unsupported PPM file format (was expecting P5 header)");

    ifs.ignore(1);

    char c;
    ifs.get(c);
    if (c == '#')
    {
      while (c != '\n')
        ifs.get(c);
    } else
      ifs.putback(c);

    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t max_val = 0;

    ifs >> width >> height >> max_val;

    if (max_val != 255)
      throw std::runtime_error("Unsupported PPM file format");

    image<std::byte> image({width, height});

    for (ptrdiff_t i = 0; i < height; ++i)
    {
      auto* const row_ptr = &image.as_mdspan()(i, 0);
      ifs.read(reinterpret_cast<char*>(row_ptr), width);
    }

    return image;
  }

  inline void export_ppm(const image<std::byte>& image, const std::filesystem::path& path)
  {
    std::ofstream ofs(path, std::ios_base::binary);

    if (!ofs)
      throw std::runtime_error("Cannot open file at path " + path.string() + " for writing");

    ofs.exceptions(std::ofstream::failbit);

    const auto [width, height] = image.extents();
    constexpr auto max_val = std::numeric_limits<std::uint8_t>::max();

    ofs << "P5" << '\n';
    ofs << width << '\n';
    ofs << height << '\n';
    ofs << static_cast<std::uint32_t>(max_val) << '\n';

    for (ptrdiff_t i = 0; i < height; ++i)
    {
      const auto* const row_ptr = &image.as_mdspan()(i, 0);
      ofs.write(reinterpret_cast<const char*>(row_ptr), width);
    }
  }
}  // namespace quxflux
