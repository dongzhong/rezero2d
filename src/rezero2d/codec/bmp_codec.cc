// Created by DONG Zhong on 2024/02/13.

#include "rezero2d/codec/bmp_codec.h"

#include "rezero2d/base/api.h"
#include "rezero2d/utils/int_operations.h"

namespace rezero {

namespace bmp {

struct BitmapFileHeader {
  std::uint32_t file_size;
  std::uint32_t reserved = 0;
  std::uint32_t offset;

  void EndianSwap() {
    file_size = ByteSwap(file_size);
    offset = ByteSwap(offset);
  }
};

static constexpr std::uint32_t kHeaderSizeV1 = 40;
static constexpr std::uint32_t kHeaderSizeV2 = 52;
static constexpr std::uint32_t kHeaderSizeV3 = 56;
static constexpr std::uint32_t kHeaderSizeV4 = 108;
static constexpr std::uint32_t kHeaderSizeV5 = 124;

static constexpr std::uint32_t kCompressionRGB = 0;
static constexpr std::uint32_t kCompressionRLE8 = 1;
static constexpr std::uint32_t kCompressionRLE4 = 2;
static constexpr std::uint32_t kCompressionBitFields = 3;
static constexpr std::uint32_t kCompressionJPEG = 4;
static constexpr std::uint32_t kCompressionPNG = 5;
static constexpr std::uint32_t kCompressionAlphaBitFields = 6;
static constexpr std::uint32_t kCompressionCMYK = 11;
static constexpr std::uint32_t kCompreesionCMYKRLE8 = 12;
static constexpr std::uint32_t kCompressionCMYKRLE4 = 13;

struct DIBHeader {
  // Version 1
  std::uint32_t header_size;
  std::int32_t width;
  std::int32_t height;
  std::uint16_t planes = 1;
  std::uint16_t bits_per_pixel;
  std::uint32_t compression_method;
  std::uint32_t image_size;
  std::uint32_t h_resolution;
  std::uint32_t v_resolution;
  std::uint32_t color_palettes_count;
  std::uint32_t important_colors_count;

  // Version 2 and 3
  union {
    std::uint32_t masks[4];
    struct {
      std::uint32_t r_mask;
      std::uint32_t g_mask;
      std::uint32_t b_mask;
      std::uint32_t a_mask;
    };
  };

  // Version 4
  struct XYZ {
    std::uint32_t x, y, z;
  };

  std::uint32_t color_space;
  XYZ r;
  XYZ g;
  XYZ b;
  std::uint32_t r_gamma;
  std::uint32_t g_gamma;
  std::uint32_t b_gamma;

  // TODO: Version 5

  inline void EndianSwap() {
    header_size = ByteSwap(header_size);
    width = ByteSwap(width);
    height = ByteSwap(height);
    planes = ByteSwap(planes);
    bits_per_pixel = ByteSwap(bits_per_pixel);
    compression_method = ByteSwap(compression_method);
    image_size = ByteSwap(image_size);
    h_resolution = ByteSwap(h_resolution);
    v_resolution = ByteSwap(v_resolution);
    color_palettes_count = ByteSwap(color_palettes_count);
    important_colors_count = ByteSwap(important_colors_count);

    r_mask = ByteSwap(r_mask);
    g_mask = ByteSwap(g_mask);
    b_mask = ByteSwap(b_mask);
    a_mask = ByteSwap(a_mask);

    color_space = ByteSwap(color_space);
    r.x = ByteSwap(r.x);
    r.y = ByteSwap(r.y);
    r.z = ByteSwap(r.z);
    g.x = ByteSwap(g.x);
    g.y = ByteSwap(g.y);
    g.z = ByteSwap(g.z);
    b.x = ByteSwap(b.x);
    b.y = ByteSwap(b.y);
    b.z = ByteSwap(b.z);
    r_gamma = ByteSwap(r_gamma);
    g_gamma = ByteSwap(g_gamma);
    b_gamma = ByteSwap(b_gamma);
  }
};

} // namespace bmp

BMPCodec::BMPCodec() = default;

BMPCodec::~BMPCodec() = default;

std::shared_ptr<Data> BMPCodec::EncodeToFileData(Format format, std::uint32_t width,
                                                 std::uint32_t height, void* data) {
  auto result = std::make_shared<Data>();

  bmp::BitmapFileHeader file_header;
  bmp::DIBHeader dib_header;

  FormatInformation format_info(format);

  dib_header.header_size = bmp::kHeaderSizeV4;
  dib_header.width = width;
  dib_header.height = height;
  dib_header.bits_per_pixel = format_info.GetBytesPerPixel() * 8;
  dib_header.compression_method = bmp::kCompressionRGB;
  dib_header.image_size = width * height * format_info.GetBytesPerPixel();
  dib_header.h_resolution = 0;
  dib_header.v_resolution = 0;
  dib_header.color_palettes_count = 0;
  dib_header.important_colors_count = 0;

  dib_header.r_mask = (format_info.HasRChannel() ? 0xFF : 0) << format_info.GetRShift();
  dib_header.g_mask = (format_info.HasGChannel() ? 0xFF : 0) << format_info.GetGShift();
  dib_header.b_mask = (format_info.HasBChannel() ? 0xFF : 0) << format_info.GetBShift();
  dib_header.a_mask = (format_info.HasAChannel() ? 0xFF : 0) << format_info.GetAShift();

  dib_header.color_space = 'Win ';
  dib_header.r = { 0, 0, 0 };
  dib_header.g = { 0, 0, 0 };
  dib_header.b = { 0, 0, 0 };
  dib_header.r_gamma = 0;
  dib_header.g_gamma = 0;
  dib_header.b_gamma = 0;

  file_header.offset = 14 + bmp::kHeaderSizeV4;
  file_header.file_size = file_header.offset + dib_header.image_size;

  if (GetEndianOrder() == Endianness::kBigEndian) {
    file_header.EndianSwap();
    dib_header.EndianSwap();
  }

  result->Init(file_header.file_size, nullptr);

  auto* p = static_cast<char*>(result->GetData());

  // File header
  *p = 'B'; ++p;
  *p = 'M'; ++p;
  std::memcpy(p, &file_header.file_size, 4); p += 4;
  p += 4; // Reserved
  std::memcpy(p, &file_header.offset, 4); p += 4;

  // DIB Header
  std::memcpy(p, &dib_header.header_size, 4); p += 4;
  std::memcpy(p, &dib_header.width, 4), p += 4;
  std::memcpy(p, &dib_header.height, 4); p += 4;
  std::memcpy(p, &dib_header.planes, 2); p += 2;
  std::memcpy(p, &dib_header.bits_per_pixel, 2); p += 2;
  std::memcpy(p, &dib_header.compression_method, 4); p += 4;
  std::memcpy(p, &dib_header.image_size, 4); p += 4;
  std::memcpy(p, &dib_header.h_resolution, 4); p += 4;
  std::memcpy(p, &dib_header.v_resolution, 4); p += 4;
  std::memcpy(p, &dib_header.color_palettes_count, 4); p += 4;
  std::memcpy(p, &dib_header.important_colors_count, 4); p += 4;

  std::memcpy(p, &dib_header.r_mask, 4); p += 4;
  std::memcpy(p, &dib_header.g_mask, 4); p += 4;
  std::memcpy(p, &dib_header.b_mask, 4); p += 4;
  std::memcpy(p, &dib_header.a_mask, 4); p += 4;

  std::memcpy(p, &dib_header.color_space, 4); p += 4;
  std::memcpy(p, &dib_header.r.x, 4); p += 4;
  std::memcpy(p, &dib_header.r.y, 4); p += 4;
  std::memcpy(p, &dib_header.r.z, 4); p += 4;
  std::memcpy(p, &dib_header.g.x, 4); p += 4;
  std::memcpy(p, &dib_header.g.y, 4); p += 4;
  std::memcpy(p, &dib_header.g.z, 4); p += 4;
  std::memcpy(p, &dib_header.b.x, 4); p += 4;
  std::memcpy(p, &dib_header.b.y, 4); p += 4;
  std::memcpy(p, &dib_header.b.z, 4); p += 4;
  std::memcpy(p, &dib_header.r_gamma, 4); p += 4;
  std::memcpy(p, &dib_header.g_gamma, 4); p += 4;
  std::memcpy(p, &dib_header.b_gamma, 4); p += 4;

  std::memcpy(p, data, width * height * format_info.GetBytesPerPixel());

  return result;
}

} // namespace rezero
