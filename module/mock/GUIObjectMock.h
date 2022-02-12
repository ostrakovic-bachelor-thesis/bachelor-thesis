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
  GUIObjectMock():
    m_isDrawCompleted(true),
    m_callbackDescription{
      .functionPtr = nullptr,
      .argument    = nullptr
    }
  {
    ON_CALL(*this, draw(_))
      .WillByDefault([&](GUI::DrawHardware drawHardware)
      {
        if (GUI::DrawHardware::DMA2D == drawHardware)
        {
          m_isDrawCompleted = false;
        }
      });

    ON_CALL(*this, isDrawCompleted())
      .WillByDefault([&](void)
      {
        return m_isDrawCompleted;
      });

    ON_CALL(*this, registerDrawCompletedCallback(_))
      .WillByDefault([&](const CallbackDescription &callbackDescription)
      {
        m_callbackDescription = callbackDescription;
      });
  }

  virtual ~GUIObjectMock() = default;

  // mock methods
  MOCK_METHOD(IFrameBuffer&, getFrameBuffer, (), (override));
  MOCK_METHOD(IFrameBuffer&, getFrameBuffer, (), (const, override));
  MOCK_METHOD(void, setFrameBuffer, (IFrameBuffer &), (override));
  MOCK_METHOD(uint16_t, getWidth, (), (const, override));
  MOCK_METHOD(uint16_t, getHeight, (), (const, override));
  MOCK_METHOD(GUI::Dimension, getDimension, (), (const, override));
  MOCK_METHOD(bool, doesContainPoint, (GUI::Point), (const, override));
  MOCK_METHOD(GUI::Position, getPosition, (GUI::Position::Tag), (const, override));
  MOCK_METHOD(void, moveToPosition, (const GUI::Position &), (override));
  MOCK_METHOD(void, draw, (GUI::DrawHardware), (override));
  MOCK_METHOD(bool, isDrawCompleted, (), (const, override));
  MOCK_METHOD(GUI::ErrorCode, getDrawingTime, (GUI::DrawHardware, uint64_t &), (const, override));
  MOCK_METHOD(void, registerDrawCompletedCallback, (const CallbackDescription &), (override));
  MOCK_METHOD(void, unregisterDrawCompletedCallback, (), (override));
  MOCK_METHOD(void, notify, (const GUI::TouchEvent &), (override));

  // fake method;
  inline void callbackDMA2DDrawCompleted(void)
  {
    m_isDrawCompleted = true;

    if (nullptr != m_callbackDescription.functionPtr)
    {
      m_callbackDescription.functionPtr(m_callbackDescription.argument);
    }
  }

private:

  bool m_isDrawCompleted;

  CallbackDescription m_callbackDescription;
};

#endif // #ifndef GUI_RECTANGLE_OBJECT_MOCK_H