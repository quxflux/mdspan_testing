#pragma once

#include <quxflux/common/wrap_mdspan.h>

#include <mio/mmap.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <typeinfo>
#include <utility>

namespace quxflux
{
  namespace detail
  {
    using type_info_identifier_pair = std::pair<const std::type_info&, std::string_view>;

    inline constexpr size_t identifier_len = 3;

    inline constexpr auto type_identifier_map = std::to_array<type_info_identifier_pair>({
      {typeid(double), "f64"},
      {typeid(float), "f32"},
      {typeid(int32_t), "i32"},
    });

    template<class ElementType, typename MmapHandleT>
    struct mmap_accessor_impl
    {
      using offset_policy = mmap_accessor_impl;
      using element_type = ElementType;
      using data_handle_type = ElementType*;
      using reference = ElementType&;

      mmap_accessor_impl(MmapHandleT&& handle) : m_handle(std::make_shared<MmapHandleT>(std::move(handle))) {}

      constexpr inline data_handle_type offset(const data_handle_type p, const size_t i) const noexcept
      {
        return p + i;
      }

      constexpr inline reference access(const data_handle_type p, const size_t i) const noexcept
      {
        // the mdspan instance will be created with a nullptr so we may use this knowledge here to find the
        // position in the memory mapped file
        const auto nullptr_offset = ((p + i) - static_cast<data_handle_type>(nullptr));
        const auto element_index = nullptr_offset * sizeof(ElementType);

        return reinterpret_cast<reference>((*m_handle)[element_index]);
      }

    private:
      std::shared_ptr<MmapHandleT> m_handle;
    };
  }  // namespace detail

  constexpr std::string_view identifier_for_type_info(const std::type_info& info)
  {
    // std::ranges::find with projection does not work because one cannot get a pointer to
    // a reference member
    for (const auto& [t_info, identifier] : detail::type_identifier_map)
      if (t_info == info)
        return identifier;

    throw std::invalid_argument("no identifier for given type");
  }

  constexpr const std::type_info& type_info_for_identifier(const std::string_view identifier)
  {
    if (const auto it = std::ranges::find(detail::type_identifier_map, identifier,
                                          &detail::type_info_identifier_pair::second);
        it != std::ranges::end(detail::type_identifier_map))
      return it->first;

    throw std::invalid_argument("no type for given identifier");
  }

  template<typename T>
    requires(std::is_const_v<T>)
  [[nodiscard]] auto create_memmapped_mdspan2d(const std::filesystem::path& path)
  {
    uint64_t n_rows = 0;
    uint64_t n_cols = 0;

    constexpr auto header_length = detail::identifier_len + sizeof(uint64_t) * 2;

    {
      mio::mmap_source mmap(path.string());
      std::istringstream iss(std::string{std::ranges::begin(mmap), std::next(std::ranges::begin(mmap), header_length)});
      iss.exceptions(std::ios_base::failbit);

      std::string identifier(detail::identifier_len, ' ');
      iss.read(identifier.data(), detail::identifier_len);

      if (type_info_for_identifier(identifier) != typeid(T))
        throw std::invalid_argument("requested element type does not match element type stored in file");

      iss.read(reinterpret_cast<char*>(&n_rows), sizeof(uint64_t));
      iss.read(reinterpret_cast<char*>(&n_cols), sizeof(uint64_t));
    }

    using accessor_t = detail::mmap_accessor_impl<T, mio::mmap_source>;
    using extents_t = NS_MDSPAN::dextents<size_t, 2>;

    return NS_MDSPAN::mdspan<T, extents_t, NS_MDSPAN::layout_right, accessor_t>(
      nullptr, extents_t{n_rows, n_cols}, accessor_t(mio::mmap_source(path.string(), header_length)));
  };
}  // namespace quxflux
