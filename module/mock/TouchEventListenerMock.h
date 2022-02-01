#ifndef TOUCH_EVENT_LISTENER_MOCK_H
#define TOUCH_EVENT_LISTENER_MOCK_H

#include "ITouchEventListener.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class TouchEventListenerMock : public ITouchEventListener
{
public:
  TouchEventListenerMock() = default;
  virtual ~TouchEventListenerMock() = default;

  // mock methods
  MOCK_METHOD(void, notify, (), (override));
};

#endif // #ifndef TOUCH_EVENT_LISTENER_MOCK_H