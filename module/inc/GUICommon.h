#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include <cstdint>


namespace GUI
{
  //! TODO This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                             = 0u,
    MEASUREMENT_NOT_AVAILABLE      = 1u,
    UNSUPPORTED_FBUFF_COLOR_FORMAT = 2u,
    Z_INDEX_ALREADY_IN_USAGE       = 3u,
    CONTAINER_FULL_ERROR           = 4u,
    DMA2D_TRANSACTION_ONGOING      = 5u,
  };

  //! TODO
  enum class DrawHardware : uint8_t
  {
    CPU   = 0u,
    DMA2D = 1u
  };

  //! TODO
  enum class ColorFormat : uint8_t
  {
    ARGB8888 = 0u,
    RGB888   = 1u,
  };

  //! TODO
  struct Position
  {
    enum class Tag : uint8_t
    {
      CENTER              = 0u,
      TOP_LEFT_CORNER     = 1u,
      TOP_RIGHT_CORNER    = 2u,
      BOTTOM_LEFT_CORNER  = 3u,
      BOTTOM_RIGHT_CORNER = 4u,
    };

    inline bool operator==(const Position &position) const
    {
      return (position.x == x) && (position.y == y) && (position.tag == tag);
    }

    inline bool operator!=(const Position &position) const
    {
      return not (position == *this);
    }

    int16_t x;
    int16_t y;
    Tag tag;
  };

  //! TODO
  struct Dimension
  {
    inline bool operator==(const Dimension &dimension) const
    {
      return (dimension.width == width) && (dimension.height == height);
    }

    inline bool operator!=(const Dimension &dimension) const
    {
      return not (dimension == *this);
    }

    uint16_t width;
    uint16_t height;
  };

  //! TODO
  struct Color
  {
    inline bool operator==(const Color &color) const
    {
      return (color.red == red) && (color.green == green) && (color.blue == blue);
    }

    inline bool operator!=(const Color &color) const
    {
      return not (color == *this);
    }

    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
}

#endif // #ifndef GUI_COMMON_H