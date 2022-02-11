#ifndef GUI_CONTAINER_MOCK_H
#define GUI_CONTAINER_MOCK_H

#include "IGUIContainer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class GUIContainerMock : public GUI::IContainer
{
public:
  GUIContainerMock() = default;
  virtual ~GUIContainerMock() = default;

  MOCK_METHOD(IFrameBuffer&, getFrameBuffer, (), (override));
  MOCK_METHOD(const IFrameBuffer&, getFrameBuffer, (), (const, override));
  MOCK_METHOD(void, setFrameBuffer, (IFrameBuffer &), (override));

  MOCK_METHOD(GUI::Position, getPosition, (GUI::Position::Tag), (const, override));

  MOCK_METHOD(void, draw, (GUI::DrawHardware), (override));
  MOCK_METHOD(bool, isDrawCompleted, (), (const, override));
  MOCK_METHOD(GUI::ErrorCode, getDrawingTime, (GUI::DrawHardware, uint64_t &), (const, override));

  MOCK_METHOD(void, registerDrawCompletedCallback, (const CallbackDescription &), (override));
  MOCK_METHOD(void, unregisterDrawCompletedCallback, (), (override));

  MOCK_METHOD(uint32_t, getCapacity, (), (const, override));
  MOCK_METHOD(uint32_t, getSize, (), (const, override));
  MOCK_METHOD(bool, isEmpty, (), (const, override));

  MOCK_METHOD(GUI::IObject*, getObject, (uint32_t), (override));
  MOCK_METHOD(GUI::ErrorCode, addObject, (GUI::IObject *, uint32_t), (override));

  MOCK_METHOD(GUI::IObject*, getEventTarget, (const GUI::TouchEvent &), (override));
  MOCK_METHOD(void, dispatchEvent, (GUI::TouchEvent &), (override));
};

#endif // #ifndef GUI_CONTAINER_MOCK_H