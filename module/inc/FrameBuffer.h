#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "IFrameBuffer.h"
#include <cstdint>


template <uint16_t t_width, uint16_t t_height, IFrameBuffer::ColorFormat t_colorFormat>
class FrameBuffer : public IFrameBuffer
{
public:

  inline uint16_t getWidth(void) const override
  {
    return t_width;
  }

  inline uint16_t getHeight(void) const override
  {
    return t_height;
  }

  inline Dimension getDimension(void) const override
  {
    return
    {
      .width  = t_width,
      .height = t_height
    };
  }

  inline ColorFormat getColorFormat(void) const override
  {
    return t_colorFormat;
  }

  inline uint32_t getSize(void) const override
  {
    return sizeof(m_frameBuffer);
  }

  inline void* getPointer(void) override
  {
    return reinterpret_cast<void*>(m_frameBuffer);
  }

  inline const void* getPointer(void) const override
  {
    return reinterpret_cast<const void*>(m_frameBuffer);
  }

  inline bool operator==(const IFrameBuffer &frameBuffer) const override
  {
    return (this == &frameBuffer);
  }

private:

  uint8_t m_frameBuffer[static_cast<uint32_t>(t_width)  *
                        static_cast<uint32_t>(t_height) *
                        IFrameBuffer::getColorFormatPixelSize(t_colorFormat)];
};

#endif // #ifndef FRAME_BUFFER_H