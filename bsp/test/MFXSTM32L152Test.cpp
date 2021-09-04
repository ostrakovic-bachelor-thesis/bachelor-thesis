#include "MFXSTM32L152.h"
#include "I2C.h"
#include <cstdint>
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class I2CMock : public I2C
{
public:

  I2CMock():
    I2C(nullptr, nullptr)
  {}

  virtual ~I2CMock() = default;

  // Mock methods
  MOCK_METHOD(ErrorCode, init, (const I2CConfig &), (override));
  MOCK_METHOD(ErrorCode, write, (uint16_t, const void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, read, (uint16_t, void *, uint32_t), (override));
  MOCK_METHOD(bool, isTransactionOngoing, (), (const, override));
  MOCK_METHOD(void, IRQHandler, (), (override));
};


class AMFXSTM32L152 : public Test
{
public:

  NiceMock<I2CMock> i2cMock;
  MFXSTM32L152 virtualMFXSTM32L152 = MFXSTM32L152(&i2cMock);

  void SetUp() override;
  void TearDown() override;
};


void AMFXSTM32L152::SetUp()
{

}

void AMFXSTM32L152::TearDown()
{

}


TEST_F(AMFXSTM32L152, DummyTest)
{
  ASSERT_THAT(true, Eq(true));
}
