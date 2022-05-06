#include "mts/config.h"

#ifdef MTS_MACOS

  #include "mts/graphics/image.h"
  #include "mts/assert.h"
  #include "helpers.h"

MTS_BEGIN_NAMESPACE
image::image(const mts::filesystem::path& filename) {
  CGDataProviderRef data_provider = CGDataProviderCreateWithFilename(filename.c_str());
  _image = CGImageCreateWithPNGDataProvider(data_provider, nullptr, true, kCGRenderingIntentDefault);
  CGDataProviderRelease(data_provider);

  if (_image) {
    CGImageRetain((CGImageRef)_image);
  }
}

image::image(const mts::filesystem::path& filename, float scale_factor)
    : image(filename) {
  _scale_factor = scale_factor;
}

image::image(const image& img) {
  _image = img._image;
  _scale_factor = img._scale_factor;

  if (_image) {
    CGImageRetain((CGImageRef)_image);
  }
}

image::image(image&& img) {
  _image = img._image;
  _scale_factor = img._scale_factor;
  img._image = nullptr;
  img._scale_factor = 1;
}

image::~image() {
  if (_image) {
    CGImageRelease((CGImageRef)_image);
  }
}

image& image::operator=(const image& img) {
  if (_image) {
    CGImageRelease((CGImageRef)_image);
    _image = nullptr;
  }

  _image = img._image;
  _scale_factor = img._scale_factor;

  if (_image) {
    CGImageRetain((CGImageRef)_image);
  }

  return *this;
}

image& image::operator=(image&& img) {
  if (_image) {
    CGImageRelease((CGImageRef)_image);
    _image = nullptr;
  }

  _image = img._image;
  _scale_factor = img._scale_factor;
  img._image = nullptr;
  img._scale_factor = 1;
  return *this;
}

bool image::is_valid() const { return _image != nullptr; }

image image::make_copy() {
  if (!is_valid()) {
    return image();
  }

  image img;
  img._image = CGImageCreateCopy((CGImageRef)_image);
  img._scale_factor = _scale_factor;
  CGImageRetain((CGImageRef)img._image);
  return img;
}

mts::size<float> image::get_size() const {
  if (!is_valid()) {
    return { 0, 0 };
  }

  float w = (float)CGImageGetWidth((CGImageRef)_image);
  float h = (float)CGImageGetHeight((CGImageRef)_image);
  return mts::size<float>(w, h) * (1.0f / _scale_factor);
}
MTS_END_NAMESPACE

#endif // MTS_MACOS
