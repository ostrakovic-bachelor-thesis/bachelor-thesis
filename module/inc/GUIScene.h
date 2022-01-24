#ifndef GUI_SCENE_H
#define GUI_SCENE_H

#include "GUISceneBase.h"
#include "IFrameBuffer.h"
#include "ArrayList.h"


namespace GUI
{
  template <uint32_t t_capacity>
  class Scene : public SceneBase
  {
  public:
    Scene(IFrameBuffer &frameBuffer);

  private:

    ArrayList<GUI::SceneBase::GUIObjectInfo,t_capacity> m_guiObjectInfoList;
  };

  template <uint32_t t_capacity>
  Scene<t_capacity>::Scene(IFrameBuffer &frameBuffer):
    SceneBase(m_guiObjectInfoList, frameBuffer)
  {}
}
#endif // #ifndef GUI_SCENE_H