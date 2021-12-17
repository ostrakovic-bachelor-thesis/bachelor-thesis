#ifndef DMA2D_MOCK_H
#define DMA2D_MOCK_H

#include "DMA2D.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class DMA2DMock : public DMA2D
{
public:

  DMA2DMock():
    DMA2D(nullptr, nullptr)
  {}

  virtual ~DMA2DMock() = default;

  inline void setReturnErrorCode(ErrorCode errorCode)
  {
    m_errorCode = errorCode;
  }

  // mock methods
  MOCK_METHOD(ErrorCode, init, (), (override));
  MOCK_METHOD(ErrorCode, fillRectangle, (const FillRectangleConfig &), (override));
  MOCK_METHOD(ErrorCode, copyBitmap, (const CopyBitmapConfig &), (override));
  MOCK_METHOD(ErrorCode, blendBitmap, (const BlendBitmapConfig &), (override));
  MOCK_METHOD(bool, isTransferOngoing, (), (override, const));
  MOCK_METHOD(void, IRQHandler, (), (override));

private:

  ErrorCode m_errorCode = ErrorCode::OK;
};

#endif // #ifndef DMA2D_MOCK_H