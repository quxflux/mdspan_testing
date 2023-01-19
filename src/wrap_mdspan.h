#pragma once

#ifdef __cpp_lib_mdspan
#include <mdspan>

#define NS_MDSPAN std

#else
#include <experimental/mdspan>

#define NS_MDSPAN std::experimental

#endif
