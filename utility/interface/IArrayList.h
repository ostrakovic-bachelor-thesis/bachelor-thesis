#ifndef I_ARRAY_LIST_H
#define I_ARRAY_LIST_H

#include "IArrayListBase.h"
#include <cstdint>


template <typename T>
class IArrayList : public IArrayListBase
{
public:
  using Iterator      = T*;
  using ConstIterator = const T*;

  virtual ~IArrayList() = default;

  virtual ErrorCode getElement(uint32_t index, T *elementPtr) const = 0;

  virtual ErrorCode addElement(const T &element) = 0;

  virtual ErrorCode addElement(uint32_t index, const T &element) = 0;

  virtual ErrorCode removeElement(uint32_t index) = 0;

  virtual Iterator getBeginIterator(void) = 0;

  virtual Iterator getEndIterator(void) = 0;

  virtual ConstIterator getBeginIterator(void) const = 0;

  virtual ConstIterator getEndIterator(void) const = 0;
};

template <typename T>
bool operator==(const IArrayList<T> &arrayListLhs, const IArrayList<T> &arrayListRhs)
{
  if (arrayListLhs.getSize() != arrayListRhs.getSize())
  {
    return false;
  }

  auto arrayListLhsIter = arrayListLhs.getBeginIterator();
  auto arrayListRhsIter = arrayListRhs.getBeginIterator();
  while (arrayListLhsIter != arrayListLhs.getEndIterator())
  {
    if (*(arrayListLhsIter++) != *(arrayListRhsIter++))
    {
      return false;
    }
  }

  return true;
}

template <typename T>
bool operator!=(const IArrayList<T> &arrayListLhs, const IArrayList<T> &arrayListRhs)
{
  return not (arrayListLhs == arrayListRhs);
}

#endif // #ifndef I_ARRAY_LIST_H