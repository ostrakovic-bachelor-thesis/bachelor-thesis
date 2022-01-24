#include "GUISceneBase.h"
#include "ArrayList.h"
#include "FrameBuffer.h"
#include "GUIObjectMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUISceneBase : public Test
{
public:

  static constexpr uint32_t RANDOM_Z_INDEX = 270u;

  NiceMock<GUIObjectMock> guiObjectMock;
  NiceMock<GUIObjectMock> guiObjectMock1;
  NiceMock<GUIObjectMock> guiObjectMock2;
  NiceMock<GUIObjectMock> guiObjectMock3;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  ArrayList<GUI::SceneBase::GUIObjectInfo,5u> guiObjectInfoList;
  GUI::SceneBase guiSceneBase = GUI::SceneBase(guiObjectInfoList, frameBuffer);

  void expectThatIGUIObjectWillBeSetToUseGUISceneFrameBuffer(GUIObjectMock &guiObjectMock, GUI::IScene &guiScene);
  void addNRandomIGUIObjectsIntoIGUIScene(GUI::IScene &guiScene, uint32_t numberOfIGUIObjects);
};

void AGUISceneBase::expectThatIGUIObjectWillBeSetToUseGUISceneFrameBuffer(GUIObjectMock &guiObjectMock, GUI::IScene &guiScene)
{
  EXPECT_CALL(guiObjectMock, setFrameBuffer(_))
    .Times(1u)
    .WillOnce([&](IFrameBuffer &frameBuffer)
    {
      ASSERT_EQ(guiScene.getFrameBuffer(), frameBuffer);
    });
}

void AGUISceneBase::addNRandomIGUIObjectsIntoIGUIScene(GUI::IScene &guiScene, uint32_t numberOfIGUIObjects)
{
  for (uint32_t i = 0u; i < numberOfIGUIObjects; ++i)
  {
    guiScene.addObject(&guiObjectMock, i);
  }
}


TEST_F(AGUISceneBase, GetCapacityReturnsMaximumNumberOfObjectsWhichCanBeAssignedToTheSceneAtOnce)
{
  constexpr uint32_t EXPECTED_GUI_SCENE_BASE_CAPACITY = 20u;
  ArrayList<GUI::SceneBase::GUIObjectInfo,EXPECTED_GUI_SCENE_BASE_CAPACITY> guiObjectInfoList;
  GUI::SceneBase guiSceneBase(guiObjectInfoList, frameBuffer);

  ASSERT_THAT(guiSceneBase.getCapacity(), Eq(EXPECTED_GUI_SCENE_BASE_CAPACITY));
}

TEST_F(AGUISceneBase, GetFrameBufferReturnsReferenceToTheFrameBufferAssociatedWithGUIScene)
{
  FrameBuffer<20, 20, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUI::SceneBase guiSceneBase(guiObjectInfoList, frameBuffer);

  ASSERT_EQ(guiSceneBase.getFrameBuffer(), frameBuffer);
}

TEST_F(AGUISceneBase, AfterCreatedDoesNotContainAnyObject)
{
  ASSERT_THAT(guiSceneBase.getObjectsCount(), Eq(0u));
}

TEST_F(AGUISceneBase, GetObjectReturnsNullptrIfNoObjectIsPresentAtTheGivenZIndexInTheScene)
{
  ASSERT_THAT(guiSceneBase.getObject(RANDOM_Z_INDEX), Eq(nullptr));
}

TEST_F(AGUISceneBase, AddObjectIncreasesCountOfObjectsInTheSceneByOneIfCalledSuccessfully)
{
  const GUI::ErrorCode errorCode = guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiSceneBase.getObjectsCount(), Eq(1u));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUISceneBase, AddObjectIfCalledSuccessfullyForcesForwardedObjectToUseTheSameFrameBufferAsTheScene)
{
  expectThatIGUIObjectWillBeSetToUseGUISceneFrameBuffer(guiObjectMock, guiSceneBase);

  const GUI::ErrorCode errorCode = guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUISceneBase, AddObjectAddsForwardedObjectAtGivenZIndexInTheScene)
{
  const GUI::ErrorCode errorCode = guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiSceneBase.getObject(RANDOM_Z_INDEX), Eq(&guiObjectMock));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUISceneBase, AddObjectFailsIfInTheSceneThereAlreadyExistsAnObjectAtGivenZIndex)
{
  guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  const GUI::ErrorCode errorCode = guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::Z_INDEX_ALREADY_IN_USAGE));
}

TEST_F(AGUISceneBase, AddObjectFailsIfThereIsNoMoreEmptySpaceLeftInTheSceneToAddNewObject)
{
  addNRandomIGUIObjectsIntoIGUIScene(guiSceneBase, guiSceneBase.getCapacity());

  const GUI::ErrorCode errorCode = guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::CONTAINER_FULL_ERROR));
}

TEST_F(AGUISceneBase, IsEmptyReturnsTrueIfThereIsNoObjectsAssignedToTheScene)
{
  ASSERT_THAT(guiSceneBase.isEmpty(), Eq(true));
  ASSERT_THAT(guiSceneBase.getObjectsCount(), Eq(0u));
}

TEST_F(AGUISceneBase, IsEmptyReturnsFalseIfThereIsAtTheLeastOneObjectsAssignedToTheScene)
{
  guiSceneBase.addObject(&guiObjectMock, RANDOM_Z_INDEX);

  ASSERT_THAT(guiSceneBase.isEmpty(), Eq(false));
  ASSERT_THAT(guiSceneBase.getObjectsCount(), Ne(0u));
}

TEST_F(AGUISceneBase, GetBeginIteratorReturnsTheSameIteratorAsGetEndIteratorIfTheSceneIsEmpty)
{
  ASSERT_THAT(guiSceneBase.getBeginIterator(), Eq(guiSceneBase.getEndIterator()));
  ASSERT_THAT(guiSceneBase.isEmpty(), Eq(true));
}

TEST_F(AGUISceneBase, GetBeginIteratorReturnsIteratorWhichPointsToObjectInfoWithTheLowestZIndexInTheScene)
{
  guiSceneBase.addObject(&guiObjectMock1, 10u);
  guiSceneBase.addObject(&guiObjectMock2, 5u);

  ASSERT_THAT(guiSceneBase.getBeginIterator()->zIndex, Eq(5u));
  ASSERT_THAT(guiSceneBase.getBeginIterator()->guiObjectPtr, Eq(&guiObjectMock2));
}

TEST_F(AGUISceneBase, IteratorPlusPlusOperationCausesMovingOfIteratorToTheObjectInfoWithTheNextLargerZIndexInTheScene)
{
  guiSceneBase.addObject(&guiObjectMock3, 9u);
  guiSceneBase.addObject(&guiObjectMock2, 5u);
  guiSceneBase.addObject(&guiObjectMock1, 2u);
  GUI::SceneBase::Iterator iterator = guiSceneBase.getBeginIterator();

  iterator++;

  ASSERT_THAT(iterator->zIndex, Eq(5u));
  ASSERT_THAT(iterator->guiObjectPtr, Eq(&guiObjectMock2));
}

TEST_F(AGUISceneBase, IteratorPlusPlusOperationWhenPerformedOnTheObjectInfoWithTheLargestZIndexTransformsIteratorToEndIterator)
{
  guiSceneBase.addObject(&guiObjectMock2, 5u);
  guiSceneBase.addObject(&guiObjectMock1, 2u);
  GUI::SceneBase::Iterator iterator = guiSceneBase.getBeginIterator();
  iterator++;

  iterator++;

  ASSERT_THAT(iterator, Eq(guiSceneBase.getEndIterator()));
}
