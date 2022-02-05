#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "IArrayList.h"


template <typename T, uint32_t t_capacity>
class ArrayList : public IArrayList<T>
{
public:

  using Iterator      = typename IArrayList<T>::Iterator;
  using ConstIterator = typename IArrayList<T>::ConstIterator;

  using ErrorCode = IArrayListBase::ErrorCode;

  inline uint32_t getCapacity(void) const override
  {
    return t_capacity;
  }

  inline bool isEmpty(void) const override
  {
    return 0u == m_size;
  }

  inline bool isFull(void) const override
  {
    return t_capacity == m_size;
  }

  inline uint32_t getSize(void) const override
  {
    return m_size;
  }

  ErrorCode getElement(uint32_t index, T *elementPtr) const override;

  ErrorCode addElement(const T &element) override;

  ErrorCode addElement(uint32_t index, const T &element) override;

  ErrorCode removeElement(uint32_t index) override;

  inline Iterator getBeginIterator(void) override
  {
    return &m_buffer[0];
  }

  inline Iterator getEndIterator(void) override
  {
    return &m_buffer[m_size];
  }

  inline ConstIterator getBeginIterator(void) const override
  {
     return &m_buffer[0];
  }

  inline ConstIterator getEndIterator(void) const override
  {
    return &m_buffer[m_size];
  }

private:

  void copyElementsForward(const T *sourcePtr, T *destinationPtr, uint32_t size);
  void copyElementsBackward(const T *sourcePtr, T *destinationPtr, uint32_t size);

  T m_buffer[t_capacity];

  uint32_t m_size = 0u;
};

template <typename T, uint32_t t_capacity>
IArrayListBase::ErrorCode ArrayList<T,t_capacity>::getElement(uint32_t index, T *elementPtr) const
{
  ErrorCode errorCode = ErrorCode::OK;

  if (index >= m_size)
  {
    errorCode = ErrorCode::OUT_OF_RANGE_ERROR;
  }

  if (nullptr == elementPtr)
  {
    errorCode = ErrorCode::NULL_POINTER_ERROR;
  }

  if (ErrorCode::OK == errorCode)
  {
    *elementPtr = m_buffer[index];
  }

  return errorCode;
}

template <typename T, uint32_t t_capacity>
IArrayListBase::ErrorCode ArrayList<T,t_capacity>::addElement(const T &element)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (m_size >= t_capacity)
  {
    errorCode = ErrorCode::CONTAINER_FULL_ERROR;
  }

  if (ErrorCode::OK == errorCode)
  {
    m_buffer[m_size++] = element;
  }

  return errorCode;
}

template <typename T, uint32_t t_capacity>
IArrayListBase::ErrorCode ArrayList<T,t_capacity>::addElement(uint32_t index, const T &element)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (index > m_size)
  {
    errorCode = ErrorCode::OUT_OF_RANGE_ERROR;
  }

  if (m_size >= t_capacity)
  {
    errorCode = ErrorCode::CONTAINER_FULL_ERROR;
  }

  if (ErrorCode::OK == errorCode)
  {
    if (0u != m_size)
    {
      copyElementsBackward(&m_buffer[m_size - 1u], &m_buffer[m_size], m_size - index);
    }

    m_buffer[index] = element;
    ++m_size;
  }

  return errorCode;
}

template <typename T, uint32_t t_capacity>
IArrayListBase::ErrorCode ArrayList<T,t_capacity>::removeElement(uint32_t index)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (index >= m_size)
  {
    errorCode = ErrorCode::OUT_OF_RANGE_ERROR;
  }

  if (ErrorCode::OK == errorCode)
  {
    --m_size;
    copyElementsForward(&m_buffer[index + 1], &m_buffer[index], m_size - index);
  }

  return errorCode;
}

template <typename T, uint32_t t_capacity>
void ArrayList<T,t_capacity>::copyElementsForward(const T *sourcePtr, T *destinationPtr, uint32_t size)
{
  for (uint32_t i = 0u; i < size; ++i)
  {
    destinationPtr[i] = sourcePtr[i];
  }
}

template <typename T, uint32_t t_capacity>
void ArrayList<T,t_capacity>::copyElementsBackward(const T *sourcePtr, T *destinationPtr, uint32_t size)
{
  for (int32_t i = 0; i > -static_cast<int32_t>(size); --i)
  {
    destinationPtr[i] = sourcePtr[i];
  }
}

#endif // #ifndef ARRAY_LIST_H