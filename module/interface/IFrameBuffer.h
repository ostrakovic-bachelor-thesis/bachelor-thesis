#ifndef I_FRAME_BUFFER_H
#define I_FRAME_BUFFER_H

#include <cstdint>


class IFrameBuffer
{
public:
  virtual ~IFrameBuffer() = default;

  enum class ColorFormat : uint8_t
  {
    ARGB8888 = 0u,
    RGB888   = 1u,
  };

  virtual uint16_t getWidth(void) const = 0;
  virtual uint16_t getHeight(void) const = 0;
  virtual uint32_t getSize(void) const = 0;
  virtual ColorFormat getColorFormat(void) const = 0;
  virtual void* getPointer(void) = 0;
  virtual const void* getPointer(void) const = 0;
  virtual bool operator==(const IFrameBuffer &frameBuffer) const = 0;

  constexpr static uint8_t getColorFormatPixelSize(ColorFormat colorFormat);
};

constexpr uint8_t IFrameBuffer::getColorFormatPixelSize(ColorFormat colorFormat)
{
  switch(colorFormat)
  {
    case ColorFormat::ARGB8888:
      return 4u;

    case ColorFormat::RGB888:
      return 3u;

    default:
      return 0u;
  }
}

#endif // #ifndef I_FRAME_BUFFER_H