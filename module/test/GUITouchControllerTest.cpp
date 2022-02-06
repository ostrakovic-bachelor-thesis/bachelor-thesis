#include "ITouchEventListener.h"
#include "IGUIContainer.h"

namespace GUI
{
  class TouchController : public ITouchEventListener
  {
  public:

    inline IContainer* getRegisteredContainer(void)
    {
      return m_containerPtr;
    }

    inline void registerContainer(IContainer *containerPtr)
    {
      m_containerPtr = containerPtr;
    }

    inline void unregisterContainer(void)
    {
      m_containerPtr = nullptr;
    }

    void notify(const TouchEvent &touchEvent) override;


  private:

    IContainer *m_containerPtr = nullptr;
  };
}

void GUI::TouchController::notify(const TouchEvent &touchEvent)
{
  if (nullptr != m_containerPtr)
  {
    m_containerPtr->getEventTarget(touchEvent);
  }
}

#include "GUIContainerMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUITouchController : public Test
{
public:

  TouchEvent RANDOM_TOUCH_EVENT = TouchEvent(0u, TouchEvent::Type::TOUCH_MOVE);
  TouchEvent START_TOUCH_EVENT  = TouchEvent(0u, TouchEvent::Type::TOUCH_START);

  GUIContainerMock guiContainerMock;
  GUI::TouchController guiTouchController = GUI::TouchController();
};

// GUI::MotionListener for notify()
// GUI::MotionListener for dispatchEvent()
// GUI::MotionEvent.consume()
// ---------------------------------------
// GUI::TouchController implements GUI::ITouchListener interface
// GUI::Scene/GUI::Container can be registered to GUI::TouchController
// GUI::TouchController, when it is notified about touch, finds out object in container, to which touch belongs.

// a) Register GUI::Container on which touch controller will operate
// b) Get registered GUI::Container returns nullptr when container is not registered
// c) Get registered GUI::Container returns registered GUI container
// d) Unregister current GUI::Container (event does not sent further)
// e) When notified about event, it generated MotionEvent and sent it to object in container
// f) Objects is container implements GUI::IMotionListener interface

// GUI::TouchController::notify()
//             |
//             |
// GUI::Container::dispatchEvent() ------------
//             |                              |
//           if no object                 valid object
//             |                              |
//             |                              |
// GUI::Container::notify() -----<---- GUI::Object::notify()

TEST_F(AGUITouchController, GetRegisteredContainerReturnsNullptrIfContainerIsNotRegistered)
{
  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(nullptr));
}

TEST_F(AGUITouchController, GetRegisteredContainerReturnsPointerToRegisteredGUIContainer)
{
  guiTouchController.registerContainer(&guiContainerMock);

  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(&guiContainerMock));
}

TEST_F(AGUITouchController, GetRegisteredContainerReturnsNullptrIfPreviouslyRegisteredContainerIsUnregistered)
{
  guiTouchController.registerContainer(&guiContainerMock);
  guiTouchController.unregisterContainer();

  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(nullptr));
}

TEST_F(AGUITouchController, NotifyDoesNotFailIfGUIContainerIsNotRegistered)
{
  guiTouchController.unregisterContainer();

  guiTouchController.notify(RANDOM_TOUCH_EVENT);

  SUCCEED();
}

TEST_F(AGUITouchController, WhenNotifiedAboutTouchStartEventItCallsGUIContainerGetEventTargetMethod)
{
  guiTouchController.registerContainer(&guiContainerMock);
  EXPECT_CALL(guiContainerMock, getEventTarget(START_TOUCH_EVENT))
    .Times(1u);

  guiTouchController.notify(START_TOUCH_EVENT);
}