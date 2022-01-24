#ifndef GUI_RECTANGLE_BASE_MOCK_H
#define GUI_RECTANGLE_BASE_MOCK_H

#include "GUIRectangleBase.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class GUIRectangleBaseMock : public GUI::RectangleBase
{
public:
  GUIRectangleBaseMock(SysTick &sysTick, IFrameBuffer &frameBuffer):
    GUI::RectangleBase(sysTick, frameBuffer)
  {}
  virtual ~GUIRectangleBaseMock() = default;

  // visibility is intentionally altered from protected to public to enable testability
  MOCK_METHOD(void , drawCPU, (), (override));
  MOCK_METHOD(void , drawDMA2D, (), (override));

};

#endif // #ifndef GUI_RECTANGLE_BASE_MOCK_H