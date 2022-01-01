#ifndef USART_LOGGER_H
#define USART_LOGGER_H

#include "USART.h"
#include "IStringBuilder.h"


class USARTLogger
{
public:

  USARTLogger(USART &usart);

  void write(const char *dataToSend);

  void write(const IStringBuilder &stringBuilder);

private:

  void waitForWriteToComplete(void);

  USART &m_usart;
};

#endif // #ifndef USART_LOGGER_H