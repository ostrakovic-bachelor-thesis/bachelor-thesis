#ifndef I_GUI_OBJECT_MOCK_H
#define I_GUI_OBJECT_MOCK_H

#include "IGUIObject.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class IGUIObjectMock : public IGUIObject
{
public:

  IGUIObjectMock()= default;
  virtual ~IGUIObjectMock() = default;

  // mock methods
  MOCK_METHOD(uint16_t, getWidth, (), (const, override));
  MOCK_METHOD(uint16_t, getHeight, (), (const, override));
  MOCK_METHOD(Dimension, getDimension, (), (const, override));
  MOCK_METHOD(Position, getPosition, (Position::Tag), (const, override));
  MOCK_METHOD(void, moveToPosition, (const Position &), (override));
  MOCK_METHOD(void, draw, (DrawHardware), (override));
};

#endif // #ifndef I_GUI_OBJECT_MOCK_H