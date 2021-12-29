#include "GUIDrawPerformanceMeasurement.h"
#include "SysTickMock.h"
#include "IGUIObjectMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIDrawPerformanceMeasurement : public Test
{
public:
  NiceMock<SysTickMock> sysTickMock;
  NiceMock<IGUIObjectMock> guiObjectMock;
  GUIDrawPerformanceMeasurement guiDrawPerformanceMeasurement = GUIDrawPerformanceMeasurement(sysTickMock);
  GUIDrawPerformanceMeasurement::GUIDrawPerformanceMeasurementConfig guiDrawPerformanceMeasurementConfig;

  uint64_t m_sysTickFunctionCallCounter;

  void expectThatGUIObjectDrawWillBeCalledWithGivenDrawHardware(IGUIObject::DrawHardware drawHardware);
  void expectThatGetSysTickWillBeCalledImmediatelyBeforeAndGetElapsedTimeInUsAfterGUIObjectDrawIsCalled(void);
  void setupSysTickReadings(uint64_t drawOperationExecutionTimeInUs);

  void SetUp() override;
};

void AGUIDrawPerformanceMeasurement::SetUp()
{
  m_sysTickFunctionCallCounter = 0u;
}

void AGUIDrawPerformanceMeasurement::expectThatGUIObjectDrawWillBeCalledWithGivenDrawHardware(IGUIObject::DrawHardware drawHardware)
{
  EXPECT_CALL(guiObjectMock, draw(drawHardware))
    .Times(1u);
  EXPECT_CALL(sysTickMock, getTicks())
    .Times(AnyNumber());
}

void AGUIDrawPerformanceMeasurement::expectThatGetSysTickWillBeCalledImmediatelyBeforeAndGetElapsedTimeInUsAfterGUIObjectDrawIsCalled(void)
{
  {
    InSequence expectCallsInSequence;

    EXPECT_CALL(sysTickMock, getTicks())
      .Times(1u);
    EXPECT_CALL(guiObjectMock, draw(_))
      .Times(1u);
    EXPECT_CALL(sysTickMock, getElapsedTimeInUs(_))
      .Times(1u);
  }
}

void AGUIDrawPerformanceMeasurement::setupSysTickReadings(uint64_t drawOperationExecutionTimeInUs)
{
  EXPECT_CALL(sysTickMock, getTicks())
    .WillRepeatedly([&] ()
    {
      return (m_sysTickFunctionCallCounter++);
    });

  EXPECT_CALL(sysTickMock, getElapsedTimeInUs(_))
    .WillRepeatedly([&, drawOperationExecutionTimeInUs] (uint64_t timestamp)
    {
      return ((m_sysTickFunctionCallCounter++) - timestamp) * drawOperationExecutionTimeInUs;
    });
}


TEST_F(AGUIDrawPerformanceMeasurement, ExecuteReturnsNotInitializedErrorIfCalledOnUninitializedObject)
{
  ASSERT_THAT(guiDrawPerformanceMeasurement.execute(), Eq(GUIDrawPerformanceMeasurement::ErrorCode::NOT_INITIALIZED));
}

TEST_F(AGUIDrawPerformanceMeasurement, GetExecutionTimeReturnsNotInitializedErrorIfCalledOnUninitializedObject)
{
  uint64_t executionTimeInUs;

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode = guiDrawPerformanceMeasurement.getExecutionTime(executionTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::NOT_INITIALIZED));
}

TEST_F(AGUIDrawPerformanceMeasurement, InitFailsWithNullPointerErrorIfGuiObjectPtrIsNullptr)
{
  guiDrawPerformanceMeasurementConfig.guiObjectPtr = nullptr;

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode =
    guiDrawPerformanceMeasurement.init(guiDrawPerformanceMeasurementConfig);

  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::NULL_POINTER_ERROR));
}

TEST_F(AGUIDrawPerformanceMeasurement, ExecuteCallsIGUIObjectDrawMethodWithDrawHardwareArgumentGivenAtTheInit)
{
  guiDrawPerformanceMeasurementConfig.guiObjectPtr = &guiObjectMock;
  guiDrawPerformanceMeasurementConfig.drawHardware = IGUIObject::DrawHardware::DMA2D;
  guiDrawPerformanceMeasurement.init(guiDrawPerformanceMeasurementConfig);
  expectThatGUIObjectDrawWillBeCalledWithGivenDrawHardware(IGUIObject::DrawHardware::DMA2D);

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode = guiDrawPerformanceMeasurement.execute();

  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::OK));
}

TEST_F(AGUIDrawPerformanceMeasurement, ExecuteCallsSysTickGetTicksBeforeAndGetElapsedTimeInUsAfterIGUIObjectDrawIsCalled)
{
  guiDrawPerformanceMeasurementConfig.guiObjectPtr = &guiObjectMock;
  guiDrawPerformanceMeasurementConfig.drawHardware = IGUIObject::DrawHardware::CPU;
  guiDrawPerformanceMeasurement.init(guiDrawPerformanceMeasurementConfig);
  expectThatGetSysTickWillBeCalledImmediatelyBeforeAndGetElapsedTimeInUsAfterGUIObjectDrawIsCalled();

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode = guiDrawPerformanceMeasurement.execute();

  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::OK));
}

TEST_F(AGUIDrawPerformanceMeasurement, GetExecutionTimeFailsWhenCalledAfterInitButBeforeAnyExecuteOperationCall)
{
  uint64_t executionTimeInUs;
  guiDrawPerformanceMeasurementConfig.guiObjectPtr = &guiObjectMock;
  guiDrawPerformanceMeasurementConfig.drawHardware = IGUIObject::DrawHardware::DMA2D;
  guiDrawPerformanceMeasurement.init(guiDrawPerformanceMeasurementConfig);

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode =
    guiDrawPerformanceMeasurement.getExecutionTime(executionTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::EXECUTION_TIME_IS_NOT_YET_ESTIMATED));
}

TEST_F(AGUIDrawPerformanceMeasurement, GetExecutionTimeReturnsTimeElapsedDuringIGUIObjectDrawOperation)
{
  constexpr uint64_t EXPECTED_DRAW_OPERATION_EXECUTION_TIME_IN_US = 1500u;
  uint64_t executionTimeInUs;
  guiDrawPerformanceMeasurementConfig.guiObjectPtr = &guiObjectMock;
  guiDrawPerformanceMeasurementConfig.drawHardware = IGUIObject::DrawHardware::DMA2D;
  guiDrawPerformanceMeasurement.init(guiDrawPerformanceMeasurementConfig);
  setupSysTickReadings(EXPECTED_DRAW_OPERATION_EXECUTION_TIME_IN_US);
  guiDrawPerformanceMeasurement.execute();

  const GUIDrawPerformanceMeasurement::ErrorCode errorCode =
    guiDrawPerformanceMeasurement.getExecutionTime(executionTimeInUs);

  ASSERT_THAT(executionTimeInUs, Eq(EXPECTED_DRAW_OPERATION_EXECUTION_TIME_IN_US));
  ASSERT_THAT(errorCode, Eq(GUIDrawPerformanceMeasurement::ErrorCode::OK));
}
