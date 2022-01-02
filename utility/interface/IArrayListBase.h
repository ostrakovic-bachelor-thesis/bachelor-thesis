#ifndef I_ARRAY_LIST_BASE_H
#define I_ARRAY_LIST_BASE_H

#include <cstdint>


class IArrayListBase
{
public:
  virtual ~IArrayListBase() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                   = 0u,
    OUT_OF_RANGE_ERROR   = 1u,
    NULL_POINTER_ERROR   = 2u,
    CONTAINER_FULL_ERROR = 3u
  };

  virtual uint32_t getCapacity(void) const = 0;
  virtual bool isEmpty(void) const = 0;
  virtual bool isFull(void) const = 0;
  virtual uint32_t getSize(void) const = 0;
};

#endif // #ifndef I_ARRAY_LIST_BASE_H