#ifndef GUI_RECTANGLE_OBJECT_MOCK_H
#define GUI_RECTANGLE_OBJECT_MOCK_H

#include "IGUIObject.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class GUIObjectMock : public GUI::IObject
{
public:
  GUIObjectMock()= default;
  virtual ~GUIObjectMock() = default;

  // mock methods
  MOCK_METHOD(IFrameBuffer&, getFrameBuffer, (), (override));
  MOCK_METHOD(IFrameBuffer&, getFrameBuffer, (), (const, override));
  MOCK_METHOD(void, setFrameBuffer, (IFrameBuffer &), (override));
  MOCK_METHOD(uint16_t, getWidth, (), (const, override));
  MOCK_METHOD(uint16_t, getHeight, (), (const, override));
  MOCK_METHOD(GUI::Dimension, getDimension, (), (const, override));
  MOCK_METHOD(GUI::Position, getPosition, (GUI::Position::Tag), (const, override));
  MOCK_METHOD(void, moveToPosition, (const GUI::Position &), (override));
  MOCK_METHOD(void, draw, (GUI::DrawHardware), (override));
  MOCK_METHOD(bool, isDrawCompleted, (), (const, override));
  MOCK_METHOD(GUI::ErrorCode, getDrawingTime, (GUI::DrawHardware, uint64_t &), (const, override));
};

#endif // #ifndef GUI_RECTANGLE_OBJECT_MOCK_H