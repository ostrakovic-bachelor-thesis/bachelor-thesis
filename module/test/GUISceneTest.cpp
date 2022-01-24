#include "GUIScene.h"
#include "FrameBuffer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(AGUIScene, CapacityIsGivenAsGUISceneCapacityTemplateArgument)
{
  constexpr uint32_t GUI_SCENE_CAPACITY = 50u;
  FrameBuffer<1u, 1u, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUI::Scene<GUI_SCENE_CAPACITY> guiScene(frameBuffer);

  ASSERT_THAT(guiScene.getCapacity(), Eq(GUI_SCENE_CAPACITY));
}
