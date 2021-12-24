#ifndef I_GUI_OBJECT_H
#define I_GUI_OBJECT_H

#include <cstdint>


class IGUIObject
{
public:
  virtual ~IGUIObject() = default;

  enum class DrawHardware : uint8_t
  {
    CPU   = 0u,
    DMA2D = 1u
  };

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

  virtual uint16_t getWidth(void) const = 0;
  virtual uint16_t getHeight(void) const = 0;
  virtual Position getPosition(Position::Tag positionTag) const = 0;
  virtual void moveToPosition(const Position &position) = 0;
  virtual void draw(DrawHardware drawHardware) = 0;
};

#endif // #ifndef I_GUI_OBJECT_H