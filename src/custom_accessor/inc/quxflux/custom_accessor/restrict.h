#pragma once

namespace quxflux
{
  constexpr auto as_restrict(auto* const ptr)
  {
#if defined(_MSVC_LANG)
    return [](auto* __restrict const p) {
      return p;
    }(ptr);
#else
    static_assert(false, "got no restrict impl");
#endif
  }
}  // namespace quxflux
