#include "USARTLogger.h"
#include <cstring>


USARTLogger::USARTLogger(USART &usart):
  m_usart(usart)
{}

void USARTLogger::write(const char *dataToSend)
{
  m_usart.write(dataToSend, strlen(dataToSend));
  waitForWriteToComplete();
}

void USARTLogger::write(const IStringBuilder &stringBuilder)
{
  m_usart.write(stringBuilder.getCString(), stringBuilder.getSize());
  waitForWriteToComplete();
}

void USARTLogger::waitForWriteToComplete(void)
{
  while (m_usart.isWriteTransactionOngoing());
}

