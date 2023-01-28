# mdspan_testing

A small test project to explore the [`std::mdspan` C++ library feature](https://en.cppreference.com/w/cpp/container/mdspan) coming up with C++23.

## notes

As there is no `std::mdspan` implementation available in the major `std` library implementations at the time of this writing, this project uses the [kokkos `mdspan` reference implementation](https://github.com/kokkos/mdspan).

## implemented examples

### 1. `image_ops` (src/image_ops)

Reads in a gray scale image, applies a [box filter](https://en.wikipedia.org/wiki/Box_blur) and writes the filtered image to disk.

`mdspan` is hereby being used to access the individual pixels of an image. Different `layout_policies` are being used to control the behavior of the filter near the borders transparently (clamp, wrap around or reflect).


## Acknowledgements

* [Jason Turners C++ Starter Project](https://github.com/cpp-best-practices/cpp_starter_project)
* input image for `example_image_ops` https://www.publicdomainpictures.net/en/view-image.php?image=197897&picture=reporter-camera
