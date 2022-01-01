#ifndef I_STRING_BUILDER_H
#define I_STRING_BUILDER_H

#include <cstdint>


class IStringBuilder
{
public:
  virtual ~IStringBuilder() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                   = 0u,
    NO_ENOUGH_FREE_SPACE = 1u,
  };

  virtual uint32_t getCapacity(void) const = 0;
  virtual uint32_t getSize(void) const = 0;
  virtual const char* getCString(void) const = 0;
  virtual ErrorCode append(const char *cStringPtr) = 0;
  virtual ErrorCode append(uint32_t number) = 0;
  virtual void reset(void) = 0;
};

#endif // #ifndef I_STRING_BUILDER_H