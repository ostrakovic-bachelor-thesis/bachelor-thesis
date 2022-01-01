#include "StringBuilder.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;


class AStringBuilder : public Test
{
public:
  static const char RANDOM_C_STRING[];
  static const char C_STRING_WHICH_CAN_NOT_FIT[];

  static constexpr uint32_t RANDOM_UINT32_NUMBER = 92165730u;
  static constexpr uint32_t NUMBER_OF_DIGITS_IN_RANDOM_UINT32_NUMBER = 8u;
  static const char RANDOM_UINT32_NUMBER_C_STRING_REPRESENTATION[];


  StringBuilder<100> stringBuilder;
};

const char AStringBuilder::RANDOM_C_STRING[] = "Random C array :D :D :D.";
const char AStringBuilder::RANDOM_UINT32_NUMBER_C_STRING_REPRESENTATION[] = "92165730";
const char AStringBuilder::C_STRING_WHICH_CAN_NOT_FIT[] =
  "Veryyyyyyyyyyyyy largeeeeeeeee C string which can not fit inside StringBuilder, "
  "because it has more than 100 characters!!!";


TEST_F(AStringBuilder, GetCapacityReturnsMaximumLengthOfAStringWhichCanBeBuildWithStringBuilder)
{
  constexpr uint32_t STRING_BUILDER_CAPACITY = 100u;
  StringBuilder<STRING_BUILDER_CAPACITY> stringBuilder;

  ASSERT_THAT(stringBuilder.getCapacity(), Eq(STRING_BUILDER_CAPACITY));
}

TEST_F(AStringBuilder, GetSizeReturnsZeroWhenCalledOnJustCreatedObject)
{
  ASSERT_THAT(stringBuilder.getSize(), Eq(0u));
}

TEST_F(AStringBuilder, GetCStringReturnsPointerToEmptyNullTerminatedCharacterArrayWhenCalledOnJustCreatedObject)
{
  ASSERT_STREQ("", stringBuilder.getCString());
}

TEST_F(AStringBuilder, AppendCStringIncreasesSizeByNumberOfCharacterAppended)
{
  IStringBuilder::ErrorCode errorCode = stringBuilder.append(RANDOM_C_STRING);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::OK));
  ASSERT_THAT(stringBuilder.getSize(), Eq(sizeof(RANDOM_C_STRING) - 1u));
}

TEST_F(AStringBuilder, AppendCStringAppendsGivenCharacterArrayToTheEndOfExistingStringBuilderContent)
{
  IStringBuilder::ErrorCode errorCode = stringBuilder.append(RANDOM_C_STRING);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::OK));
  ASSERT_STREQ(RANDOM_C_STRING, stringBuilder.getCString());
}

TEST_F(AStringBuilder, AppendCStringFailsWithNoEnoughFreeSpaceErrorIfStingBuilderDoesNotHaveEnoughFreeSpaceToFitIt)
{
  IStringBuilder::ErrorCode errorCode = stringBuilder.append(C_STRING_WHICH_CAN_NOT_FIT);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::NO_ENOUGH_FREE_SPACE));
}

TEST_F(AStringBuilder, AppendUint32NumberIncreasesSizeByNumberOfDigitsWhichCompoundTheNumber)
{
  IStringBuilder::ErrorCode errorCode = stringBuilder.append(RANDOM_UINT32_NUMBER);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::OK));
  ASSERT_THAT(stringBuilder.getSize(), Eq(NUMBER_OF_DIGITS_IN_RANDOM_UINT32_NUMBER));
}

TEST_F(AStringBuilder, AppendUint32NumberAppendsGivenNumberToTheEndOfExistingStringBuilderContent)
{
  IStringBuilder::ErrorCode errorCode = stringBuilder.append(RANDOM_UINT32_NUMBER);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::OK));
  ASSERT_STREQ(RANDOM_UINT32_NUMBER_C_STRING_REPRESENTATION, stringBuilder.getCString());
}

TEST_F(AStringBuilder, AppendUint32NumberFailsWithNoEnoughFreeSpaceErrorIfStingBuilderDoesNotHaveEnoughFreeSpaceToFitIt)
{
  constexpr uint32_t UINT32_NUMBER_WHICH_CAN_NOT_FIT = 987654321u;
  StringBuilder<5> stringBuilder;

  IStringBuilder::ErrorCode errorCode = stringBuilder.append(UINT32_NUMBER_WHICH_CAN_NOT_FIT);

  ASSERT_THAT(errorCode, Eq(IStringBuilder::ErrorCode::NO_ENOUGH_FREE_SPACE));
}

TEST_F(AStringBuilder, SequenceOfFewAppendCallsResultsInExpectedCStringAndSize)
{
  const char EXPECTED_C_STRING[] = "Happy new year 2022 !!!";
  stringBuilder.append("Happy new year ");
  stringBuilder.append(2022u);
  stringBuilder.append(" !!!");

  ASSERT_STREQ(EXPECTED_C_STRING, stringBuilder.getCString());
  ASSERT_THAT(stringBuilder.getSize(), Eq(sizeof(EXPECTED_C_STRING) - 1u));
}

TEST_F(AStringBuilder, ResetBringsStringBuilderInTheSameStateAsItWasJustAfterItHadBeenConstructed)
{
  stringBuilder.append(RANDOM_C_STRING);
  stringBuilder.append(RANDOM_UINT32_NUMBER);

  stringBuilder.reset();

  ASSERT_THAT(stringBuilder.getSize(), Eq(0u));
  ASSERT_STREQ("", stringBuilder.getCString());
}