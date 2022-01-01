#include "USARTLogger.h"
#include "USARTMock.h"
#include "StringBuilder.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <cstring>


using namespace ::testing;


class AnUSARTLogger : public Test
{
public:

  static const char RANDOM_C_STRING[];

  NiceMock<USARTMock> usartMock;
  USARTLogger usartLogger = USARTLogger(usartMock);

  bool m_areExpectedDataSentViaUSART;
  uint32_t m_isWriteTransactionCompletedCounter;
  bool m_isWriteTransactionCompleted;

  void expectThatGivenDataWillBeSentViaUSART(const uint8_t *dataToBeSent, uint32_t lengthOfData);
  void assertThatExpectedDataAreSentViaUSART(void);
  void setupUSARTIsWriteTransactionOngoingReadings(void);
  void assertThatWriteTransactionIsCompleted(void);

  void SetUp() override;
};

const char AnUSARTLogger::RANDOM_C_STRING[] = "Random null terminated character array.";

void AnUSARTLogger::SetUp()
{
  m_areExpectedDataSentViaUSART        = true;
  m_isWriteTransactionCompletedCounter = 0u;
  m_isWriteTransactionCompleted        = false;

  setupUSARTIsWriteTransactionOngoingReadings();
}

void AnUSARTLogger::expectThatGivenDataWillBeSentViaUSART(const uint8_t *dataToBeSent, uint32_t lengthOfData)
{
  EXPECT_CALL(usartMock, write(_, _))
    .Times(1u)
    .WillOnce([&, dataToBeSent, lengthOfData](const void *messagePtr, uint32_t messageLen)
    {
      m_areExpectedDataSentViaUSART &= (messageLen == lengthOfData);

      for (uint32_t i = 0u; i < lengthOfData; ++i)
      {
        m_areExpectedDataSentViaUSART &= (reinterpret_cast<const uint8_t*>(messagePtr)[i] == dataToBeSent[i]);
      }

      return USART::ErrorCode::OK;
    });
}

void AnUSARTLogger::assertThatExpectedDataAreSentViaUSART(void)
{
  ASSERT_THAT(m_areExpectedDataSentViaUSART, Eq(true));
}

void AnUSARTLogger::setupUSARTIsWriteTransactionOngoingReadings(void)
{
  ON_CALL(usartMock, isWriteTransactionOngoing())
    .WillByDefault([&](void)
    {
      m_isWriteTransactionCompletedCounter++;
      m_isWriteTransactionCompleted = (10u == m_isWriteTransactionCompletedCounter);

      return not m_isWriteTransactionCompleted;
    });
}

void AnUSARTLogger::assertThatWriteTransactionIsCompleted(void)
{
  ASSERT_THAT(m_isWriteTransactionCompleted, Eq(true));
}


TEST_F(AnUSARTLogger, WriteSendsForwardedNullTerminatedCharacterArrayOverUSARTTxLine)
{
  expectThatGivenDataWillBeSentViaUSART(reinterpret_cast<const uint8_t*>(RANDOM_C_STRING), strlen(RANDOM_C_STRING));

  usartLogger.write(RANDOM_C_STRING);

  assertThatExpectedDataAreSentViaUSART();
}

TEST_F(AnUSARTLogger, WriteNullTerminatedCharacterArrayWaitsForWriteTransactionToCompleteBeforeReturning)
{
  usartLogger.write(RANDOM_C_STRING);

  assertThatWriteTransactionIsCompleted();
}

TEST_F(AnUSARTLogger, WriteSendsContentOfStringBuilderOverUSARTTxLine)
{
  StringBuilder<50> stringBuilder;
  stringBuilder.append("String builder with ");
  stringBuilder.append(50u);
  stringBuilder.append(" character capacity.");
  expectThatGivenDataWillBeSentViaUSART(reinterpret_cast<const uint8_t*>(stringBuilder.getCString()), stringBuilder.getSize());

  usartLogger.write(stringBuilder);

  assertThatExpectedDataAreSentViaUSART();
}

TEST_F(AnUSARTLogger, WriteSendBuilderWaitsForWriteTransactionToCompleteBeforeReturning)
{
  StringBuilder<50> stringBuilder;
  stringBuilder.append("Random string.");

  usartLogger.write(stringBuilder);

  assertThatWriteTransactionIsCompleted();
}