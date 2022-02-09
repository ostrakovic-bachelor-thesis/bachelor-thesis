#ifndef GUI_TOUCH_EVENT_LISTENER_MOCK_H
#define GUI_TOUCH_EVENT_LISTENER_MOCK_H

#include "IGUITouchEventListener.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class GUITouchEventListenerMock : public GUI::ITouchEventListener
{
public:
  GUITouchEventListenerMock() = default;
  virtual ~GUITouchEventListenerMock() = default;

  // mock methods
  MOCK_METHOD(void, notify, (const GUI::TouchEvent &), (override));
};

#endif // #ifndef GUI_TOUCH_EVENT_LISTENER_MOCK_H