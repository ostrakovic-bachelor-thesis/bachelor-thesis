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

  NiceMock<GUIObjectMock> guiObjectMock;
  NiceMock<GUIObjectMock> guiObjectMock1;
  NiceMock<GUIObjectMock> guiObjectMock2;
  NiceMock<GUIObjectMock> guiObjectMock3;
  ArrayList<GUI::Container::ObjectInfo,5u> guiContainerObjectInfoList;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUI::Container guiContainer = GUI::Container(guiContainerObjectInfoList, frameBuffer);

  uint32_t m_lastZIndex;

  void addNRandomIGUIObjectsIntoIGUIContainer(GUI::Container &guiContainer, uint32_t numberOfIGUIObjects);
  void expectThatGUIObjectWillBeForcedToUseGUIContainerFrameBuffer(
    GUIObjectMock &guiObjectMock,
    GUI::Container &guiContainer);
  void expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithCPUDrawingHardware(GUI::Container &guiContainer);
  void expectThatAllGUIObjectsStoredInContainerWillBeDrawnWithDMA2DDrawingHardware(GUI::Container &guiContainer);
  void expectThatGUIObjectsWillBeDrawnInSequenceFromLowerToHigherZIndex(GUI::Container &guiContainer);
  void expectThatDrawingOfTheNextGUIObjectWillNotStartBeforeOngoingIsFinished(GUI::Container &guiContainer);
  void expectThatDrawCompletedCallbackWillBeRegisteredForGUIObject(GUIObjectMock &guiObjectMock);
  void simulateCallingOfDMA2DDrawCompletedCallbackForEachGUIObjectInTheContainer(GUI::Container &guiContainer);
  void expectThatOnlyDrawForTheGUIObjectWithTheLowestZIndexWillBeCalled(GUI::Container &guiContainer);

  void SetUp() override;
};

void AGUIContainer::SetUp()
{
  m_lastZIndex = 0u;
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