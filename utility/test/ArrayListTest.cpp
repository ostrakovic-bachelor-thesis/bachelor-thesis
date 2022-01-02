#include "ArrayList.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnArrayList : public Test
{
public:

  static constexpr uint32_t RANDOM_VALUE = 97531024u;

  ArrayList<uint32_t, 20u> uint32ArrayList;

  template <typename T>
  void assertThatElementAtIndexNIsEqualTo(IArrayList<T> &arrayList, uint32_t index, T value);
  template <typename T>
  void assertThatFirstElementInTheListIsEqualTo(IArrayList<T> &arrayList, T value);
  template <typename T>
  void assertThatLastElementInTheListIsEqualTo(IArrayList<T> &arrayList, T value);
  template <typename T>
  void addValueNTimesInArrayList(IArrayList<T> &arrayList, uint32_t times, T value);
};

constexpr uint32_t AnArrayList::RANDOM_VALUE;

template <typename T>
void AnArrayList::assertThatElementAtIndexNIsEqualTo(IArrayList<T> &arrayList, uint32_t index, T value)
{
  T element;
  const IArrayListBase::ErrorCode errorCode = arrayList.getElement(index, &element);

  ASSERT_THAT(element, Eq(value));
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

template <typename T>
void AnArrayList::assertThatFirstElementInTheListIsEqualTo(IArrayList<T> &arrayList, T value)
{
  assertThatElementAtIndexNIsEqualTo(arrayList, 0u, value);
}

template <typename T>
void AnArrayList::assertThatLastElementInTheListIsEqualTo(IArrayList<T> &arrayList, T value)
{
  assertThatElementAtIndexNIsEqualTo(arrayList, (arrayList.getSize() - 1u), value);
}

template <typename T>
void AnArrayList::addValueNTimesInArrayList(IArrayList<T> &arrayList, uint32_t times, T value)
{
  for (uint32_t i = 0u; i < times; ++i)
  {
    arrayList.addElement(value);
  }
}


TEST_F(AnArrayList, GetCapacityReturnsMaximumNumberOfElementsWhichCanBeStoredInArrayList)
{
  constexpr uint32_t ARRAY_LIST_CAPACITY = 10u;
  ArrayList<char, ARRAY_LIST_CAPACITY> charArrayList;

  ASSERT_THAT(charArrayList.getCapacity(), Eq(ARRAY_LIST_CAPACITY));
}

TEST_F(AnArrayList, HasSizeZeroAfterItHasBeenCreated)
{
  ASSERT_THAT(uint32ArrayList.getSize(), Eq(0u));
}

TEST_F(AnArrayList, GetElementFailsWithOutOfRangeErrorIfYouTryToAccessElementAtInvalidIndex)
{
  uint32_t element;
  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.getElement(1u, &element);

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OUT_OF_RANGE_ERROR));
}

TEST_F(AnArrayList, AddElementIncreasesSizeByOneIfCallWasSuccessfulAkaWithoutErrors)
{
  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(RANDOM_VALUE);

  ASSERT_THAT(uint32ArrayList.getSize(), Eq(1u));
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, IsEmptyReturnsFalseIfSizeIsNotZero)
{
  uint32ArrayList.addElement(RANDOM_VALUE);

  ASSERT_THAT(uint32ArrayList.getSize(), Ne(0u));
  ASSERT_THAT(uint32ArrayList.isEmpty(), Eq(false));
}

TEST_F(AnArrayList, IsEmptyReturnsTrueIfSizeIsZero)
{
  ASSERT_THAT(uint32ArrayList.getSize(), Eq(0u));
  ASSERT_THAT(uint32ArrayList.isEmpty(), Eq(true));
}

TEST_F(AnArrayList, GetElementReturnsAddedElementInArrayList)
{
  constexpr uint32_t ELEMENT_ADDED_IN_ARRAY_LIST = 10u;
  uint32ArrayList.addElement(ELEMENT_ADDED_IN_ARRAY_LIST);

  uint32_t element;
  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.getElement(0u, &element);

  ASSERT_THAT(element, Eq(ELEMENT_ADDED_IN_ARRAY_LIST));
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, GetElementFailsWithNullPointerErrorIfYouForwardNullPointerAsPointerToElement)
{
  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.getElement(1u, nullptr);

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::NULL_POINTER_ERROR));
}

TEST_F(AnArrayList, AddElementAddsTheElementAtTheEndOfTheList)
{
  const IArrayListBase::ErrorCode errorCode1 = uint32ArrayList.addElement(10u);
  const IArrayListBase::ErrorCode errorCode2 = uint32ArrayList.addElement(20u);

  assertThatFirstElementInTheListIsEqualTo(uint32ArrayList, 10u);
  assertThatLastElementInTheListIsEqualTo(uint32ArrayList, 20u);
  ASSERT_THAT(errorCode1, Eq(IArrayListBase::ErrorCode::OK));
  ASSERT_THAT(errorCode2, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, AddElementFailsWithContainerFullErrorIfThereIsNoMoreEmptySpaceToAddNewElementInArrayList)
{
  addValueNTimesInArrayList(uint32ArrayList, uint32ArrayList.getCapacity(), RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(10u);

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::CONTAINER_FULL_ERROR));
}

TEST_F(AnArrayList, AddElementAtIndexFailsWithOutOfRangeErrorIfYouTryToAddElementAtInvalidIndex)
{
  addValueNTimesInArrayList(uint32ArrayList, 10u, RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement((uint32ArrayList.getSize() + 1u), 10u);

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OUT_OF_RANGE_ERROR));
}

TEST_F(AnArrayList, AddElementAtIndexFailsWithContainerFullErrorIfThereIsNoMoreEmptySpaceToAddNewElementInArrayList)
{
  addValueNTimesInArrayList(uint32ArrayList, uint32ArrayList.getCapacity(), RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(0u, 10u);

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::CONTAINER_FULL_ERROR));
}

TEST_F(AnArrayList, AddElementAtIndexAddsElementAtGivenIndexInArrayList)
{
  addValueNTimesInArrayList(uint32ArrayList, uint32ArrayList.getCapacity() - 1u, RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(5u, 10u);

  assertThatElementAtIndexNIsEqualTo(uint32ArrayList, 5u, 10u);
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, AddElementAtIndexIncreasesSizeByOneIfCallWasSuccessfulAkaWithoutErrors)
{
  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(0u, RANDOM_VALUE);

  ASSERT_THAT(uint32ArrayList.getSize(), Eq(1u));
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, AddElementAtIndexShiftAllElementsFromThatIndexAndAfterByOnePositionToTheRight)
{
  uint32ArrayList.addElement(1u);
  uint32ArrayList.addElement(2u);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.addElement(0u, 10u);

  assertThatElementAtIndexNIsEqualTo(uint32ArrayList, 1u, 1u);
  assertThatElementAtIndexNIsEqualTo(uint32ArrayList, 2u, 2u);
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, RemoveElementAtIndexFailsWithOutOfRangeErrorIfYouTryToRemoveElementAtInvalidIndex)
{
  addValueNTimesInArrayList(uint32ArrayList, 10u, RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.removeElement((uint32ArrayList.getSize() + 1u));

  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OUT_OF_RANGE_ERROR));
}

TEST_F(AnArrayList, RemoveElementAtIndexDecreasesSizeByOneIfCallWasSuccessfulAkaWithoutErrors)
{
  constexpr uint32_t INITIAL_ARRAY_LIST_SIZE = 5u;
  addValueNTimesInArrayList(uint32ArrayList, INITIAL_ARRAY_LIST_SIZE, RANDOM_VALUE);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.removeElement(0u);

  ASSERT_THAT(uint32ArrayList.getSize(), Eq(INITIAL_ARRAY_LIST_SIZE - 1u));
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, RemoveElementAtIndexShiftAllElementsFromThatIndexAndAfterByOnePositionToTheLeft)
{
  uint32ArrayList.addElement(100u);
  uint32ArrayList.addElement(0u);
  uint32ArrayList.addElement(1u);
  uint32ArrayList.addElement(2u);

  const IArrayListBase::ErrorCode errorCode = uint32ArrayList.removeElement(1u);

  assertThatElementAtIndexNIsEqualTo(uint32ArrayList, 1u, 1u);
  assertThatElementAtIndexNIsEqualTo(uint32ArrayList, 2u, 2u);
  ASSERT_THAT(errorCode, Eq(IArrayListBase::ErrorCode::OK));
}

TEST_F(AnArrayList, IsFullReturnsFalseIfSizeIsNotEqualToArrayListCapacity)
{
   addValueNTimesInArrayList(uint32ArrayList, 10u, RANDOM_VALUE);

  ASSERT_THAT(uint32ArrayList.getSize(), Ne(uint32ArrayList.getCapacity()));
  ASSERT_THAT(uint32ArrayList.isFull(), Eq(false));
}

TEST_F(AnArrayList, IsFullReturnsTruefSizeIsEqualToArrayListCapacity)
{
  addValueNTimesInArrayList(uint32ArrayList, uint32ArrayList.getCapacity(), RANDOM_VALUE);

  ASSERT_THAT(uint32ArrayList.getSize(), Eq(uint32ArrayList.getCapacity()));
  ASSERT_THAT(uint32ArrayList.isFull(), Eq(true));
}