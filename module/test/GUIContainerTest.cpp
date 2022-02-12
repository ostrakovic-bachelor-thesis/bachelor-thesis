#include "GUIContainer.h"
#include "ArrayList.h"
#include "FrameBuffer.h"
#include "GUIObjectMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIContainer : public Test
{
public:

  static constexpr uint32_t RANDOM_Z_INDEX            = 270u;
  static constexpr uint32_t GUI_OBJECT_MOCK_1_Z_INDEX = 10u;
  static constexpr uint32_t GUI_OBJECT_MOCK_2_Z_INDEX = 1u;
  const GUI::Point RANDOM_GUI_POINT =
  {
    .x = 10,
    .y = 10
  };
  const GUI::TouchEvent ONE_TOUCH_POINT_TOUCH_EVENT =
    GUI::TouchEvent(40u, GUI::TouchEvent::Type::TOUCH_MOVE);
  const GUI::TouchEvent NO_TOUCH_POINTS_TOUCH_EVENT =
    GUI::TouchEvent(100u, GUI::TouchEvent::Type::TOUCH_START);
  const GUI::TouchEvent TWO_TOUCH_POINT_TOUCH_EVENT =
    GUI::TouchEvent(290u, GUI::TouchEvent::Type::TOUCH_STOP);

  NiceMock<GUIObjectMock> guiObjectMock;
  NiceMock<GUIObjectMock> guiObjectMock1;
  NiceMock<GUIObjectMock> guiObjectMock2;
  NiceMock<GUIObjectMock> guiObjectMock3;
  ArrayList<GUI::Container::ObjectInfo,5u> guiContainerObjectInfoList;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUI::Container guiContainer = GUI::Container(guiContainerObjectInfoList, frameBuffer);

  GUI::Container::CallbackDescription callbackDescription;
  ArrayList<GUI::Point,2u> touchPoints;
  uint32_t m_lastZIndex;
  uint32_t m_expectedDrawingTimeInUs;
  bool m_isCallbackCalled;

  void addNRandomIGUIObjectsIntoIGUIContainer(GUI::Container &guiContainer, uint32_t numberOfIGUIObjects);
  void expectThatGUIObjectWillBeForcedToUseGUIContainerFrameBuffer(
    GUIObjectMock &guiObjectMock,
    GUI::Container &guiContainer);
  void expectThatAllObjectsInContainerWillBeFordedToUseFrameBuffer(
    GUI::Container &guiContainer,
    IFrameBuffer &newFrameBuffer);
  void expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithCPUDrawingHardware(GUI::Container &guiContainer);
  void expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithDMA2DDrawingHardware(GUI::Container &guiContainer);
  void expectThatGUIObjectsWillBeDrawnInSequenceFromLowerToHigherZIndex(GUI::Container &guiContainer);
  void expectThatDrawingOfTheNextGUIObjectWillNotStartBeforeOngoingIsFinished(GUI::Container &guiContainer);
  void expectThatDrawCompletedCallbackWillBeRegisteredForGUIObject(GUIObjectMock &guiObjectMock);
  void simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(GUI::Container &guiContainer);
  void expectThatOnlyDrawForTheGUIObjectWithTheLowestZIndexWillBeCalled(GUI::Container &guiContainer);
  void expectThatGetDrawingTimeWillBeCalledForEachObjectInContainer(
    GUI::Container &guiContainer,
    GUI::DrawHardware drawHardware);
  void  setupGUIObjectGetDrawingTimeReadings(
    GUIObjectMock &guiObjectMock,
    GUI::DrawHardware drawHardware,
    uint64_t expectedDrawingTimeInUs,
    GUI::ErrorCode errorCode);
  void expectThatDoesContainPointWillBeCalledOnceAndWillReturn(GUIObjectMock &guiObjectMock, bool returnValue);
  void expectThatDoesContainPointWillNeverBeCalled(GUIObjectMock &guiObjectMock);
  void expectThatObjectWillBeNotified(GUIObjectMock &guiObjectMock, GUI::TouchEvent touchEvent);
  void expectThatObjectWillNotBeNotified(GUIObjectMock &guiObjectMock);
  void onCallOfDoesContainPointReturn(GUIObjectMock &guiObjectMock, bool returnValue);
  void setupGetCPUDrawingTimeReadingForContainerObjects(GUI::Container &guiContainer);
  void setupGetDMA2DDrawingTimeReadingForContainerObjects(GUI::Container &guiContainer);
  void assertThatDrawingTimeIsEqualToExpectedOne(uint64_t drawingTimeInUs);
  void assertThatCallbackIsNotCalled(void);
  void assertThatCallbackIsCalled(void);

  void SetUp() override;
};

void AGUIContainer::SetUp()
{
  m_lastZIndex = 0u;
  m_expectedDrawingTimeInUs = 0u;
  m_isCallbackCalled = false;

  callbackDescription =
  {
    .functionPtr = [](void *argument) { *reinterpret_cast<bool*>(argument) = true; },
    .argument = &m_isCallbackCalled
  };

  // inject one GUI::Point into touch event
  const_cast<IArrayList<GUI::Point>&>(ONE_TOUCH_POINT_TOUCH_EVENT.getTouchPoints()).addElement(RANDOM_GUI_POINT);

  // inject two GUI::Point-s into touch event
  const_cast<IArrayList<GUI::Point>&>(TWO_TOUCH_POINT_TOUCH_EVENT.getTouchPoints()).addElement(RANDOM_GUI_POINT);
  const_cast<IArrayList<GUI::Point>&>(TWO_TOUCH_POINT_TOUCH_EVENT.getTouchPoints()).addElement(RANDOM_GUI_POINT);
}

void AGUIContainer::addNRandomIGUIObjectsIntoIGUIContainer(GUI::Container &guiContainer, uint32_t numberOfIGUIObjects)
{
  for (uint32_t i = 0u; i < numberOfIGUIObjects; ++i)
  {
    guiContainer.addObject(&guiObjectMock, i);
  }
}

void AGUIContainer::expectThatGUIObjectWillBeForcedToUseGUIContainerFrameBuffer(
  GUIObjectMock &guiObjectMock,
  GUI::Container &guiContainer)
{
  EXPECT_CALL(guiObjectMock, setFrameBuffer(_))
    .Times(1u)
    .WillOnce([&](IFrameBuffer &frameBuffer)
    {
      ASSERT_EQ(guiContainer.getFrameBuffer(), frameBuffer);
    });
}

void AGUIContainer::expectThatAllObjectsInContainerWillBeFordedToUseFrameBuffer(
  GUI::Container &guiContainer,
  IFrameBuffer &newFrameBuffer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    EXPECT_CALL(*guiObjectMockPtr, setFrameBuffer(_))
      .Times(1u)
      .WillOnce([&](IFrameBuffer &frameBuffer)
      {
        ASSERT_EQ(newFrameBuffer, frameBuffer);
      });
  }
}

void AGUIContainer::expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithCPUDrawingHardware(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    EXPECT_CALL(*guiObjectMockPtr, draw(GUI::DrawHardware::CPU))
      .Times(1u);
  }
}

void AGUIContainer::expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithDMA2DDrawingHardware(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    EXPECT_CALL(*guiObjectMockPtr, draw(GUI::DrawHardware::DMA2D))
      .Times(1u);
  }
}

void AGUIContainer::expectThatGUIObjectsWillBeDrawnInSequenceFromLowerToHigherZIndex(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    ON_CALL(*guiObjectMockPtr, draw(_))
      .WillByDefault([&, zIndex = it.getZIndex()](GUI::DrawHardware drawHardware)
      {
        ASSERT_THAT(zIndex, Gt(m_lastZIndex));
        m_lastZIndex = zIndex;
      });
  }
}

void AGUIContainer::expectThatDrawingOfTheNextGUIObjectWillNotStartBeforeOngoingIsFinished(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator();)
  {
    GUIObjectMock *previousGuiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    it++;
    if (it != guiContainer.getEndIterator())
    {
      GUIObjectMock *currentGuiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
      ON_CALL(*currentGuiObjectMockPtr, draw(_))
        .WillByDefault([=](GUI::DrawHardware drawHardware)
        {
          ASSERT_THAT(previousGuiObjectMockPtr->isDrawCompleted(), true);
        });
    }
  }
}

void AGUIContainer::expectThatDrawCompletedCallbackWillBeRegisteredForGUIObject(GUIObjectMock &guiObjectMock)
{
  EXPECT_CALL(guiObjectMock, registerDrawCompletedCallback(_))
    .Times(1u);
}

void AGUIContainer::simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(GUI::Container &guiContainer)
{
  // simulate reporting of DMA2D draw completed callback for each object in the container
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    guiObjectMockPtr->callbackDMA2DDrawCompleted();
  }
}

void AGUIContainer::expectThatOnlyDrawForTheGUIObjectWithTheLowestZIndexWillBeCalled(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getBeginIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*(guiContainer.getBeginIterator()));
    EXPECT_CALL(*guiObjectMockPtr, draw(_))
      .Times((guiContainer.getBeginIterator() == it) ? 1u : 0u);
  }
}

void AGUIContainer::expectThatGetDrawingTimeWillBeCalledForEachObjectInContainer(
  GUI::Container &guiContainer,
  GUI::DrawHardware drawHardware)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    EXPECT_CALL(*guiObjectMockPtr, getDrawingTime(drawHardware, _))
      .Times(1u);
  }
}

void AGUIContainer::expectThatDoesContainPointWillBeCalledOnceAndWillReturn(GUIObjectMock &guiObjectMock, bool returnValue)
{
  EXPECT_CALL(guiObjectMock, doesContainPoint(_))
    .Times(1u)
    .WillOnce(Return(returnValue));
}

void AGUIContainer::expectThatDoesContainPointWillNeverBeCalled(GUIObjectMock &guiObjectMock)
{
  EXPECT_CALL(guiObjectMock, doesContainPoint(_))
    .Times(0u);
}

void AGUIContainer::expectThatObjectWillBeNotified(GUIObjectMock &guiObjectMock, GUI::TouchEvent touchEvent)
{
  EXPECT_CALL(guiObjectMock3, notify(touchEvent))
    .Times(1u);
}

void AGUIContainer::expectThatObjectWillNotBeNotified(GUIObjectMock &guiObjectMock)
{
  EXPECT_CALL(guiObjectMock3, notify(_))
    .Times(0u);
}

void AGUIContainer::onCallOfDoesContainPointReturn(GUIObjectMock &guiObjectMock, bool returnValue)
{
  ON_CALL(guiObjectMock, doesContainPoint(_))
    .WillByDefault(Return(returnValue));
}

void AGUIContainer::setupGUIObjectGetDrawingTimeReadings(
  GUIObjectMock &guiObjectMock,
  GUI::DrawHardware drawHardware,
  uint64_t expectedDrawingTimeInUs,
  GUI::ErrorCode errorCode)
{
  ON_CALL(guiObjectMock, getDrawingTime(drawHardware, _))
    .WillByDefault([&, expectedDrawingTimeInUs, errorCode](GUI::DrawHardware drawHardware, uint64_t &drawingTimeInUs)
    {
      drawingTimeInUs = expectedDrawingTimeInUs;
      m_expectedDrawingTimeInUs += drawingTimeInUs;

      return errorCode;
    });
}

void AGUIContainer::setupGetCPUDrawingTimeReadingForContainerObjects(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    setupGUIObjectGetDrawingTimeReadings(
      *guiObjectMockPtr,
      GUI::DrawHardware::CPU,
      50u,
      GUI::ErrorCode::OK);
  }
}

void AGUIContainer::setupGetDMA2DDrawingTimeReadingForContainerObjects(GUI::Container &guiContainer)
{
  for (auto it = guiContainer.getBeginIterator(); it != guiContainer.getEndIterator(); it++)
  {
    GUIObjectMock *guiObjectMockPtr = reinterpret_cast<GUIObjectMock*>(*it);
    setupGUIObjectGetDrawingTimeReadings(
      *guiObjectMockPtr,
      GUI::DrawHardware::DMA2D,
      110u,
      GUI::ErrorCode::OK);
  }
}

void AGUIContainer::assertThatDrawingTimeIsEqualToExpectedOne(uint64_t drawingTimeInUs)
{
  ASSERT_THAT(drawingTimeInUs, Eq(m_expectedDrawingTimeInUs));
}

void AGUIContainer::assertThatCallbackIsNotCalled(void)
{
  ASSERT_THAT(m_isCallbackCalled, Eq(false));
}

void AGUIContainer::assertThatCallbackIsCalled(void)
{
  ASSERT_THAT(m_isCallbackCalled, Eq(true));
}


TEST_F(AGUIContainer, GetCapacityReturnsMaximumNumberOfObjectsWhichContainerCanHoldAtOnce)
{
  constexpr uint32_t EXPECTED_GUI_CONTAINER_CAPACITY = 20u;
  ArrayList<GUI::Container::ObjectInfo,EXPECTED_GUI_CONTAINER_CAPACITY> guiContainerObjectInfoList;
  GUI::Container guiContainer(guiContainerObjectInfoList, frameBuffer);

  ASSERT_THAT(guiContainer.getCapacity(), Eq(EXPECTED_GUI_CONTAINER_CAPACITY));
}

TEST_F(AGUIContainer, GetFrameBufferReturnsReferenceToTheFrameBufferAssociatedWithTheContainer)
{
  FrameBuffer<50, 50, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUI::Container guiContainer(guiContainerObjectInfoList, frameBuffer);

  ASSERT_EQ(guiContainer.getFrameBuffer(), frameBuffer);
}

TEST_F(AGUIContainer, HasZeroSizeAfterItHasBeenCreated)
{
  ASSERT_THAT(guiContainer.getSize(), Eq(0u));
}

TEST_F(AGUIContainer, IsEmptyWhenContainerSizeIsZero)
{
  ASSERT_THAT(guiContainer.getSize(), Eq(0u));
  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
}

TEST_F(AGUIContainer, GetObjectReturnsNullptrIfNoObjectIsPresentAtTheGivenZIndexInTheContainer)
{
  ASSERT_THAT(guiContainer.getObject(RANDOM_Z_INDEX), Eq(nullptr));
}

TEST_F(AGUIContainer, AddObjectFailsIfForwardedPointerToGUIObjectIsNullptr)
{
  const GUI::ErrorCode errorCode = guiContainer.addObject(nullptr, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::ARGUMENT_NULL_POINTER));
}

TEST_F(AGUIContainer, AddObjectIncreasesContainerSizeByOneIfCalledSuccessfully)
{
  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiContainer.getSize(), Eq(1u));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, IsNotEmptyWhenContainerSizeIsNotZero)
{
  guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiContainer.getSize(), Ne(0u));
  ASSERT_THAT(guiContainer.isEmpty(), Eq(false));
}

TEST_F(AGUIContainer, AddObjectAddsForwardedObjectToTheContainerAtGivenZIndex)
{
  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiContainer.getObject(RANDOM_Z_INDEX), Eq(&guiObjectMock));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, AddObjectIfCalledSuccessfullyRegistersDrawCompletedCallbackForTheGivenGUIObject)
{
  expectThatDrawCompletedCallbackWillBeRegisteredForGUIObject(guiObjectMock);

  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, AddObjectIfCalledSuccessfullyForcesForwardedObjectToUseTheSameFrameBufferAsTheContainer)
{
  expectThatGUIObjectWillBeForcedToUseGUIContainerFrameBuffer(guiObjectMock, guiContainer);

  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, SetFrameBufferSetsNewFrameBufferWhichWillBeUsedByGUIContainer)
{
  FrameBuffer<30, 40, IFrameBuffer::ColorFormat::RGB888> newFrameBuffer;

  guiContainer.setFrameBuffer(newFrameBuffer);

  ASSERT_EQ(guiContainer.getFrameBuffer(), newFrameBuffer);
}


TEST_F(AGUIContainer, SetFrameBufferIfCalledSuccessfullyForcesAllObjectsInTheContainerToStartToUseNewFrameBuffer)
{
  FrameBuffer<30, 40, IFrameBuffer::ColorFormat::RGB888> newFrameBuffer;
  guiContainer.addObject(&guiObjectMock1, GUI_OBJECT_MOCK_1_Z_INDEX);
  guiContainer.addObject(&guiObjectMock2, GUI_OBJECT_MOCK_2_Z_INDEX);
  expectThatAllObjectsInContainerWillBeFordedToUseFrameBuffer(guiContainer, newFrameBuffer);

  guiContainer.setFrameBuffer(newFrameBuffer);
}

TEST_F(AGUIContainer, GetPositionWithAnyTagReturnsPositionWithThatTag)
{
  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::BOTTOM_LEFT_CORNER);

  ASSERT_THAT(position.tag, Eq(GUI::Position::Tag::BOTTOM_LEFT_CORNER));
}

TEST_F(AGUIContainer, GetPositionTopLeftCornerAlwaysReturnsXPosAndYPosEqualToZero)
{
  constexpr GUI::Position EXPECTED_GUI_CONTAINER_POSITION =
  {
    .x   = 0,
    .y   = 0,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };

  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_CONTAINER_POSITION));
}

TEST_F(AGUIContainer, GetPositionTopRightCornerAlwaysReturnsXPosEqualsToFrameBufferWidthAndYPosEqualsToZero)
{
  const GUI::Position EXPECTED_GUI_CONTAINER_POSITION =
  {
    .x   = static_cast<int16_t>(guiContainer.getFrameBuffer().getWidth()),
    .y   = 0,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER
  };

  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::TOP_RIGHT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_CONTAINER_POSITION));
}

TEST_F(AGUIContainer, GetPositionBottomLeftCornerAlwaysReturnsXPosEqualsToZeroAndYPosEqualsToFrameBufferHeight)
{
  const GUI::Position EXPECTED_GUI_CONTAINER_POSITION =
  {
    .x   = 0,
    .y   = static_cast<int16_t>(guiContainer.getFrameBuffer().getHeight()),
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER
  };

  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::BOTTOM_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_CONTAINER_POSITION));
}

TEST_F(AGUIContainer, GetPositionBottomRightCornerAlwaysReturnsXPosEqualsToFrameBufferWidthAndYPosEqualsToFrameBufferHeight)
{
  const GUI::Position EXPECTED_GUI_CONTAINER_POSITION =
  {
    .x   = static_cast<int16_t>(guiContainer.getFrameBuffer().getWidth()),
    .y   = static_cast<int16_t>(guiContainer.getFrameBuffer().getHeight()),
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };

  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_CONTAINER_POSITION));
}

TEST_F(AGUIContainer, GetPositionCenterAlwaysReturnsXPosEqualsToHalfFrameBufferWidthAndYPosEqualsToHalfFrameBufferHeight)
{
  const GUI::Position EXPECTED_GUI_CONTAINER_POSITION =
  {
    .x   = static_cast<int16_t>(guiContainer.getFrameBuffer().getWidth() / 2u) ,
    .y   = static_cast<int16_t>(guiContainer.getFrameBuffer().getHeight() / 2u),
    .tag = GUI::Position::Tag::CENTER
  };

  const GUI::Position position = guiContainer.getPosition(GUI::Position::Tag::CENTER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_CONTAINER_POSITION));
}

TEST_F(AGUIContainer, AddObjectFailsIfInTheContainerThereAlreadyExistsAnObjectAtGivenZIndex)
{
  guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::Z_INDEX_ALREADY_IN_USAGE));
}

TEST_F(AGUIContainer, GetObjectReturnsPointerToTheCorrectObjectWhenMoreThenOneObjectAreAddedToTheContainer)
{
  guiContainer.addObject(&guiObjectMock1, GUI_OBJECT_MOCK_1_Z_INDEX);
  guiContainer.addObject(&guiObjectMock2, GUI_OBJECT_MOCK_2_Z_INDEX);

  ASSERT_THAT(guiContainer.getObject(GUI_OBJECT_MOCK_1_Z_INDEX), &guiObjectMock1);
  ASSERT_THAT(guiContainer.getObject(GUI_OBJECT_MOCK_2_Z_INDEX), &guiObjectMock2);
}

TEST_F(AGUIContainer, AddObjectFailsIfThereIsNoMoreEmptySpaceLeftToAddNewObject)
{
  addNRandomIGUIObjectsIntoIGUIContainer(guiContainer, guiContainer.getCapacity());

  const GUI::ErrorCode errorCode = guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::CONTAINER_FULL_ERROR));
}

TEST_F(AGUIContainer, GetBeginIteratorReturnsTheSameIteratorAsGetEndIteratorIfTheContainerIsEmpty)
{
  ASSERT_THAT(guiContainer.getBeginIterator(), Eq(guiContainer.getEndIterator()));
  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
}

TEST_F(AGUIContainer, GetBeginIteratorReturnsDifferentIteratorFromGetEndIteratorIfTheContainerIsNotEmpty)
{
  guiContainer.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiContainer.getBeginIterator(), Ne(guiContainer.getEndIterator()));
  ASSERT_THAT(guiContainer.isEmpty(), Eq(false));
}

TEST_F(AGUIContainer, GetBeginIteratorReturnsIteratorWhichPointsToObjectWithTheLowestZIndexInTheContainer)
{
  guiContainer.addObject(&guiObjectMock1, 10u);
  guiContainer.addObject(&guiObjectMock2, 5u);

  ASSERT_THAT(*(guiContainer.getBeginIterator()), Eq(&guiObjectMock2));
}

TEST_F(AGUIContainer, IteratorGetZIndexReturnsZIndexOfTheObjectToWhichIteratorReferences)
{
  constexpr uint32_t EXPECTED_Z_INDEX = 210u;
  guiContainer.addObject(&guiObjectMock, EXPECTED_Z_INDEX);
  GUI::Container::Iterator iterator = guiContainer.getBeginIterator();

  ASSERT_THAT(iterator.getZIndex(), Eq(EXPECTED_Z_INDEX));
}

TEST_F(AGUIContainer, IteratorPlusPlusOperationCausesMovingOfIteratorToTheObjectWithTheNextLargerZIndexInTheContainer)
{
  guiContainer.addObject(&guiObjectMock3, 9u);
  guiContainer.addObject(&guiObjectMock2, 5u);
  guiContainer.addObject(&guiObjectMock1, 2u);
  GUI::Container::Iterator iterator = guiContainer.getBeginIterator();

  iterator++;

  ASSERT_THAT(*iterator, Eq(&guiObjectMock2));
}

TEST_F(AGUIContainer, IteratorPlusPlusOperationOnTheIteratorWhichRefersToTheLastObjectInContainerTransformsItIntoEndIterator)
{
  guiContainer.addObject(&guiObjectMock2, 5u);
  guiContainer.addObject(&guiObjectMock1, 2u);
  GUI::Container::Iterator iterator = guiContainer.getBeginIterator();
  iterator++;

  iterator++;

  ASSERT_THAT(iterator, Eq(guiContainer.getEndIterator()));
}

TEST_F(AGUIContainer, DrawWithCPUOfEmptyContainerWillNotCauseFault)
{
  guiContainer.draw(GUI::DrawHardware::CPU);

  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
  SUCCEED();
}

TEST_F(AGUIContainer, DrawWithDMA2DOfEmptyContainerWillNotCauseFault)
{
  guiContainer.draw(GUI::DrawHardware::DMA2D);

  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
  SUCCEED();
}

TEST_F(AGUIContainer, DrawWithCPUDrawsAllGUIObjectsAddedToTheContainerWithCPUDrawingHardware)
{
  guiContainer.addObject(&guiObjectMock3, 3u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock1, 2u);
  expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithCPUDrawingHardware(guiContainer);

  guiContainer.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIContainer, DrawWithCPUDrawsGUIObjectsInSequenceFromLowerToHigherZIndex)
{
  guiContainer.addObject(&guiObjectMock3, 4u);
  guiContainer.addObject(&guiObjectMock2, 11u);
  guiContainer.addObject(&guiObjectMock1, 1u);
  expectThatGUIObjectsWillBeDrawnInSequenceFromLowerToHigherZIndex(guiContainer);

  guiContainer.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIContainer, DrawWithCPUWillNotStartDrawingOfTheNextGUIObjectIfDrawingOfTheCurrentOneIsNotFinished)
{
  guiContainer.addObject(&guiObjectMock3, 3u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock1, 2u);
  expectThatDrawingOfTheNextGUIObjectWillNotStartBeforeOngoingIsFinished(guiContainer);

  guiContainer.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIContainer, IsDrawCompletedReturnsImmediatelyTrueAfterDrawingWithCPU)
{
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock1, 2u);
  guiContainer.draw(GUI::DrawHardware::CPU);

  ASSERT_THAT(guiContainer.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIContainer, DrawWithDMA2DOnlyStartsDrawingOfGUIObjectWithTheLowestZIndex)
{
  guiContainer.addObject(&guiObjectMock2, 5u);
  guiContainer.addObject(&guiObjectMock1, 20u);
  expectThatOnlyDrawForTheGUIObjectWithTheLowestZIndexWillBeCalled(guiContainer);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
}

TEST_F(AGUIContainer, IsDrawCompletedReturnsFalseIfDrawCompletedCallbackHasNotBeenCalledForAllObjectsWhenDrawingWithDMA2D)
{
  guiContainer.addObject(&guiObjectMock2, 5u);
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.draw(GUI::DrawHardware::DMA2D);

  ASSERT_THAT(guiContainer.isDrawCompleted(), Eq(false));
}

TEST_F(AGUIContainer, IsDrawCompletedReturnsTrueAfterAllObjectFromTheContainerHaveBeenDrawn)
{
  guiContainer.addObject(&guiObjectMock3, 50u);
  guiContainer.addObject(&guiObjectMock2, 1u);
  guiContainer.addObject(&guiObjectMock1, 30u);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);

  ASSERT_THAT(guiContainer.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIContainer, DrawWithDMA2DDrawsAllGUIObjectsAddedToTheContainerWithDMA2DDrawingHardware)
{
  guiContainer.addObject(&guiObjectMock3, 5u);
  guiContainer.addObject(&guiObjectMock2, 1u);
  guiContainer.addObject(&guiObjectMock1, 20u);
  expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithDMA2DDrawingHardware(guiContainer);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);
}

TEST_F(AGUIContainer, DrawWithDMA2DDrawsGUIObjectsInSequenceFromLowerToHigherZIndex)
{
  guiContainer.addObject(&guiObjectMock3, 60u);
  guiContainer.addObject(&guiObjectMock2, 15u);
  guiContainer.addObject(&guiObjectMock1, 200u);
  expectThatGUIObjectsWillBeDrawnInSequenceFromLowerToHigherZIndex(guiContainer);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);
}

TEST_F(AGUIContainer, DrawWithDMA2DWillNotStartDrawingOfTheNextGUIObjectIfDrawingOfTheCurrentOneIsNotFinished)
{
  guiContainer.addObject(&guiObjectMock3, 50u);
  guiContainer.addObject(&guiObjectMock2, 1u);
  guiContainer.addObject(&guiObjectMock1, 30u);
  expectThatDrawingOfTheNextGUIObjectWillNotStartBeforeOngoingIsFinished(guiContainer);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);
}

TEST_F(AGUIContainer, GetDrawingTimeWithCPUGetsCPUDrawingTimeOfEachObjectInContainer)
{
  guiContainer.addObject(&guiObjectMock3, 10u);
  guiContainer.addObject(&guiObjectMock1, 35u);
  guiContainer.addObject(&guiObjectMock2, 45u);
  expectThatGetDrawingTimeWillBeCalledForEachObjectInContainer(guiContainer, GUI::DrawHardware::CPU);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, GetDrawingTimeWithDMA2DGetsDMA2DDrawingTimeOfEachObjectInContainer)
{
  guiContainer.addObject(&guiObjectMock3, 15u);
  guiContainer.addObject(&guiObjectMock1, 30u);
  guiContainer.addObject(&guiObjectMock2, 5u);
  expectThatGetDrawingTimeWillBeCalledForEachObjectInContainer(guiContainer, GUI::DrawHardware::DMA2D);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, GetDrawingTimeWithCPUReturnsSumOfAllContainerObjectsCPUDrawingTimes)
{
  guiContainer.addObject(&guiObjectMock3, 50u);
  guiContainer.addObject(&guiObjectMock1, 30u);
  guiContainer.addObject(&guiObjectMock2, 1u);
  setupGetCPUDrawingTimeReadingForContainerObjects(guiContainer);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  assertThatDrawingTimeIsEqualToExpectedOne(drawingTimeInUs);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, GetDrawingTimeWithDMA2DReturnsSumOfAllContainerObjectsDMA2DDrawingTimes)
{
  guiContainer.addObject(&guiObjectMock3, 1u);
  guiContainer.addObject(&guiObjectMock2, 110u);
  guiContainer.addObject(&guiObjectMock1, 45u);
  setupGetDMA2DDrawingTimeReadingForContainerObjects(guiContainer);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  assertThatDrawingTimeIsEqualToExpectedOne(drawingTimeInUs);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIContainer, GetDrawingTimeWithCPUFailsIfDrawingTimeOfAnyGUIContainerObjectsIsNotAvailable)
{
  guiContainer.addObject(&guiObjectMock1, 30u);
  guiContainer.addObject(&guiObjectMock2, 1u);
  setupGetCPUDrawingTimeReadingForContainerObjects(guiContainer);
  guiContainer.addObject(&guiObjectMock3, 50u);
  setupGUIObjectGetDrawingTimeReadings(
    guiObjectMock3,
    GUI::DrawHardware::CPU,
    0u,
    GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE));
}

TEST_F(AGUIContainer, GetDrawingTimeWithDMA2DFailsIfDrawingTimeOfAnyGUIContainerObjectsIsNotAvailable)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 100u);
  setupGetDMA2DDrawingTimeReadingForContainerObjects(guiContainer);
  guiContainer.addObject(&guiObjectMock3, 70u);
  setupGUIObjectGetDrawingTimeReadings(
    guiObjectMock3,
    GUI::DrawHardware::DMA2D,
    0u,
    GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiContainer.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE));
}

TEST_F(AGUIContainer, DrawWithDMA2DDoesNotDirectlyCallRegisteredCallback)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 100u);
  guiContainer.registerDrawCompletedCallback(callbackDescription);

  guiContainer.draw(GUI::DrawHardware::DMA2D);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIContainer, DrawingWithDMA2DWillNotCauseFaultIfDrawCompletedCallbackIsNotRegistered)
{
  guiContainer.addObject(&guiObjectMock1, 40u);
  guiContainer.addObject(&guiObjectMock2, 30u);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);

  SUCCEED();
}

TEST_F(AGUIContainer, WhenDrawWithDMA2DIsFinishedRegisteredDrawCompletedCallbackIsCalled)
{
  guiContainer.addObject(&guiObjectMock1, 40u);
  guiContainer.addObject(&guiObjectMock2, 30u);
  guiContainer.registerDrawCompletedCallback(callbackDescription);

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);

  assertThatCallbackIsCalled();
}

TEST_F(AGUIContainer, WhenDrawingWithDMA2DPreviouslyRegisteredCallbackIsNotCalledIfLaterItIsUnregistered)
{
  guiContainer.addObject(&guiObjectMock1, 5u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.registerDrawCompletedCallback(callbackDescription);
  guiContainer.unregisterDrawCompletedCallback();

  guiContainer.draw(GUI::DrawHardware::DMA2D);
  simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(guiContainer);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIContainer, DrawWithCPUDoesNotFailIfDrawCompletedCallbackIsNotRegistered)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 10u);

  guiContainer.draw(GUI::DrawHardware::CPU);

  SUCCEED();
}

TEST_F(AGUIContainer, DrawWithCPUCallsRegisteredDrawCompletedCallbackBeforeReturningExecutionToCaller)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.registerDrawCompletedCallback(callbackDescription);

  guiContainer.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsCalled();
}

TEST_F(AGUIContainer, DrawWithCPUDoesNotCallPreviouslyRegisteredCallbackIfLaterItIsUnregistered)
{
  guiContainer.addObject(&guiObjectMock1, 15u);
  guiContainer.addObject(&guiObjectMock2, 25u);
  guiContainer.registerDrawCompletedCallback(callbackDescription);
  guiContainer.unregisterDrawCompletedCallback();

  guiContainer.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIContainer, DrawWithCPUCallsRegisteredDrawCompletedCallbackEvenWhenThereIsNoObjectInContainer)
{
  guiContainer.registerDrawCompletedCallback(callbackDescription);

  guiContainer.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsCalled();
  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
}

TEST_F(AGUIContainer, DrawWithDMA2DCallsRegisteredDrawCompletedCallbackDirectlyWhenThereIsNoObjectInContainer)
{
  guiContainer.registerDrawCompletedCallback(callbackDescription);

  guiContainer.draw(GUI::DrawHardware::DMA2D);

  assertThatCallbackIsCalled();
  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
}

TEST_F(AGUIContainer, GetEventTargetReturnsNullPointerIfContainerIsEmpty)
{
  const GUI::IObject *eventTargetPtr = guiContainer.getEventTarget(ONE_TOUCH_POINT_TOUCH_EVENT);

  ASSERT_THAT(eventTargetPtr, Eq(nullptr));
  ASSERT_THAT(guiContainer.isEmpty(), Eq(true));
}

TEST_F(AGUIContainer, GetEventTargetReturnsPointerToObjectWithTheHighestZIndexInContainerForWhichDoesContainPointReturnsTrue)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock3, 5u);
  onCallOfDoesContainPointReturn(guiObjectMock1, false);
  onCallOfDoesContainPointReturn(guiObjectMock2, true);
  onCallOfDoesContainPointReturn(guiObjectMock3, true);

  const GUI::IObject *eventTargetPtr = guiContainer.getEventTarget(ONE_TOUCH_POINT_TOUCH_EVENT);

  ASSERT_THAT(eventTargetPtr, Eq(&guiObjectMock2));
}

TEST_F(AGUIContainer, GetEventTargetReturnsNullPointerIfMethodDoesContainPointReturnsFalseForEachObjectInTheContainer)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock3, 5u);
  onCallOfDoesContainPointReturn(guiObjectMock1, false);
  onCallOfDoesContainPointReturn(guiObjectMock2, false);
  onCallOfDoesContainPointReturn(guiObjectMock3, false);

  const GUI::IObject *eventTargetPtr = guiContainer.getEventTarget(ONE_TOUCH_POINT_TOUCH_EVENT);

  ASSERT_THAT(eventTargetPtr, Eq(nullptr));
}

TEST_F(AGUIContainer, GetEventTargetDoesNotCallDoesContainPointOnAnyObjectInContainerIfTouchEventDoesNotContainTouchPoints)
{
  guiContainer.addObject(&guiObjectMock1, 20u);
  guiContainer.addObject(&guiObjectMock2, 10u);
  guiContainer.addObject(&guiObjectMock3, 5u);
  expectThatDoesContainPointWillNeverBeCalled(guiObjectMock1);
  expectThatDoesContainPointWillNeverBeCalled(guiObjectMock2);
  expectThatDoesContainPointWillNeverBeCalled(guiObjectMock3);

  const GUI::IObject *eventTargetPtr = guiContainer.getEventTarget(NO_TOUCH_POINTS_TOUCH_EVENT);

  ASSERT_THAT(eventTargetPtr, Eq(nullptr));
}

TEST_F(AGUIContainer, GetEventTargetInTheCaseOfMultiTouchEventWillMatchGUIObjectEvenIfOnlyOneTouchPointIsContainedByIt)
{
  guiContainer.addObject(&guiObjectMock, 10u);
  EXPECT_CALL(guiObjectMock, doesContainPoint(_))
    .Times(2u)
    .WillOnce(Return(false)) // doesContainPoint() will return false for the first call
    .WillOnce(Return(true)); // doesContainPoint() will return true for the second call

  const GUI::IObject *eventTargetPtr = guiContainer.getEventTarget(TWO_TOUCH_POINT_TOUCH_EVENT);

  ASSERT_THAT(eventTargetPtr, Eq(&guiObjectMock));
}

TEST_F(AGUIContainer, DispatchEventSucceedWithoutDoingAnythingIfContainerIsEmpty)
{
  GUI::TouchEvent touchEvent(10u, GUI::TouchEvent::Type::TOUCH_START);

  guiContainer.dispatchEvent(touchEvent);

  SUCCEED();
}

TEST_F(AGUIContainer, DispatchEventCallsNotifyOnGUIObjectFromContainerIfItIsEventTargetObject)
{
  guiContainer.addObject(&guiObjectMock1, 5u);
  guiContainer.addObject(&guiObjectMock2, 15u);
  guiContainer.addObject(&guiObjectMock3, 10u);
  GUI::TouchEvent touchEvent(10u, GUI::TouchEvent::Type::TOUCH_START, touchPoints, &guiObjectMock3);
  expectThatObjectWillBeNotified(guiObjectMock3, touchEvent);

  guiContainer.dispatchEvent(touchEvent);
}

TEST_F(AGUIContainer, DispatchEventDoesNotCallNotifyOnGUIObjectsFromContainerIfTheyAreNotEventTargetObject)
{
  guiContainer.addObject(&guiObjectMock1, 20);
  guiContainer.addObject(&guiObjectMock2, 5u);
  guiContainer.addObject(&guiObjectMock3, 15u);
  GUI::TouchEvent touchEvent(10u, GUI::TouchEvent::Type::TOUCH_START, touchPoints, &guiObjectMock2);
  expectThatObjectWillNotBeNotified(guiObjectMock1);
  expectThatObjectWillNotBeNotified(guiObjectMock3);

  guiContainer.dispatchEvent(touchEvent);
}