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

#endif // #ifndef I_ARRAY_LIST_H