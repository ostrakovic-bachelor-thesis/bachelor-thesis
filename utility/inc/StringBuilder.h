#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include "IStringBuilder.h"


template <uint32_t t_capacity>
class StringBuilder : public IStringBuilder
{
public:

  inline uint32_t getCapacity(void) const override
  {
    return t_capacity;
  }

  inline uint32_t getSize(void) const override
  {
    return m_size;
  }

  inline const char* getCString(void) const override
  {
    return reinterpret_cast<const char*>(m_data);
  }

  ErrorCode append(const char *cStringPtr) override;

  ErrorCode append(uint32_t number) override;

  void reset(void) override;

private:

  template <typename T>
  void swap(T &lhsValue, T &rhsValue);

  template <typename Iterator>
  void reverseOrder(Iterator startIterator, Iterator endIterator);

  inline bool isFull(void) const
  {
    return t_capacity == m_size;
  }

  uint32_t m_size = 0u;

  uint8_t m_data[t_capacity + 1u] = { '\0' };
};


template <uint32_t t_capacity>
IStringBuilder::ErrorCode StringBuilder<t_capacity>::append(const char *cStringPtr)
{
  const uint32_t oldSize = m_size;
  ErrorCode errorCode    = ErrorCode::OK;

  while (('\0' != *cStringPtr) && (m_size < t_capacity))
  {
    m_data[m_size++] = *(cStringPtr++);
  }

  // if end of C string is not reached, it means that StringBuild does not have enough free space to fit it
  if ('\0' != *cStringPtr)
  {
    m_size    = oldSize;
    errorCode = ErrorCode::NO_ENOUGH_FREE_SPACE;
  }

  m_data[m_size] = '\0';

  return errorCode;
}

template <uint32_t t_capacity>
IStringBuilder::ErrorCode StringBuilder<t_capacity>::append(uint32_t number)
{
  ErrorCode errorCode = ErrorCode::NO_ENOUGH_FREE_SPACE;

  if (not isFull())
  {
    uint8_t * const endIterator         = &m_data[t_capacity];
    uint8_t * const startNumberIterator = &m_data[m_size];
    uint8_t *endNumberIterator          = &m_data[m_size];

    do
    {
      *(endNumberIterator++) = (number % 10u) + '0';
      number /= 10u;
    } while ((0u != number) && (endNumberIterator != endIterator));

    // if the whole number is converted, it means StringBuild did have enough free space to fit it
    if (0u == number)
    {
      errorCode = ErrorCode::OK;
      reverseOrder(startNumberIterator, endNumberIterator);
    }
    else
    {
      endNumberIterator = startNumberIterator;
    }

    m_size = endNumberIterator - &m_data[0];
    *endNumberIterator = '\0';
  }

  return errorCode;
}

template <uint32_t t_capacity>
void StringBuilder<t_capacity>::reset(void)
{
  m_size         = 0u;
  m_data[m_size] = '\0';
}

template <uint32_t t_capacity>
template <typename T>
void StringBuilder<t_capacity>::swap(T &lhsValue, T &rhsValue)
{
  T temp   = lhsValue;
  lhsValue = rhsValue;
  rhsValue = temp;
}

template <uint32_t t_capacity>
template <typename Iterator>
void StringBuilder<t_capacity>::reverseOrder(Iterator startIterator, Iterator endIterator)
{
  while (startIterator < endIterator)
  {
    swap(*(startIterator++), *(--endIterator));
  }
}

#endif // #ifndef STRING_BUILDER_H