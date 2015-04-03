/*-------------------------------------------------------------------------
 *
 * value_test.cpp
 * file description
 *
 * Copyright(c) 2015, CMU
 *
 * /n-store/tests/common/value_test.cpp
 *
 *-------------------------------------------------------------------------
 */

#include "gtest/gtest.h"

#include "harness.h"

namespace nstore {
namespace test {

//===--------------------------------------------------------------------===//
// Value Tests
//===--------------------------------------------------------------------===//

#include "harness.h"

#include "common/value.h"
#include "common/value_factory.h"
#include "common/value_peeker.h"
#include "common/serializer.h"

#include <cfloat>
#include <limits>

void deserDecHelper(Value nv, ValueType &vt,
                    TTInt &value, std::string &str) {
    vt    = ValuePeeker::PeekValueType(nv);
    value = ValuePeeker::PeekDecimal(nv);
    str   = ValuePeeker::PeekDecimalString(nv);
}

TEST(ValueTest, DeserializeDecimal)
{
    int64_t scale = 1000000000000;
   std::string str;

    ValueType vt;
    TTInt value;
    Value nv;

    nv = ValueFactory::GetDecimalValueFromString("-0");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(0));
    // Decimals in Volt are currently hardwired with 12 fractional
    // decimal places.
    ASSERT_EQ(str, "0.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("0");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(0));
    ASSERT_EQ(str, "0.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("0.0");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(0));
    ASSERT_EQ(str, "0.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("1");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt("1000000000000"));
    ASSERT_EQ(str, "1.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("-1");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt("-1000000000000"));
    ASSERT_EQ(str, "-1.000000000000");

    // min value
    nv = ValueFactory::GetDecimalValueFromString("-9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");   //38 digits
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt("-9999999999"  //10 digits
            "9999999999"   //20 digits
            "9999999999"   //30 digits
            "99999999"));
    ASSERT_FALSE(strcmp(str.c_str(), "-9999999999"  //10 digits
            "9999999999"   //20 digits
            "999999.9999"   //30 digits
            "99999999"));

    // max value
    nv = ValueFactory::GetDecimalValueFromString("9999999999"  //10 digits
            "9999999999"   //20 digits
            "999999.9999"   //30 digits
            "99999999");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt("9999999999"  //10 digits
            "9999999999"   //20 digits
            "9999999999"   //30 digits
            "99999999"));
    ASSERT_FALSE(strcmp(str.c_str(), "9999999999"  //10 digits
            "9999999999"   //20 digits
            "999999.9999"   //30 digits
            "99999999"));

    nv = ValueFactory::GetDecimalValueFromString("1234");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(1234 * scale));
    ASSERT_EQ(str, "1234.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("12.34");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(static_cast<int64_t>(12340000000000)));
    ASSERT_EQ(str, "12.340000000000");

    nv = ValueFactory::GetDecimalValueFromString("-1234");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(-1234 * scale));
    ASSERT_EQ(str, "-1234.000000000000");

    nv = ValueFactory::GetDecimalValueFromString("-12.34");
    deserDecHelper(nv, vt, value, str);
    ASSERT_FALSE(nv.IsNull());
    ASSERT_EQ(vt, VALUE_TYPE_DECIMAL);
    ASSERT_EQ(value, TTInt(static_cast<int64_t>(-12340000000000)));
    ASSERT_EQ(str, "-12.340000000000");

    // illegal deserializations
    try {
        // too few digits
        nv = ValueFactory::GetDecimalValueFromString("");
        ASSERT_EQ(0,1);
    }
    catch(...) {
    }

    try {
        // too many digits
        nv = ValueFactory::GetDecimalValueFromString("11111111111111111111111111111");
        ASSERT_EQ(0,1);
    }
    catch (...) {
    }

    try {
        // too much precision
        nv = ValueFactory::GetDecimalValueFromString("999999999999999999999999999.999999999999");
        ASSERT_EQ(0,1);
    }
    catch (...) {
    }

    try {
        // too many decimal points
        nv = ValueFactory::GetDecimalValueFromString("9.9.9");
        ASSERT_EQ(0,1);
    }
    catch (...) {
    }

    try {
        // too many decimal points
        nv = ValueFactory::GetDecimalValueFromString("..0");
        ASSERT_EQ(0,1);
    }
    catch (...) {
    }

    try {
        // invalid character
        nv = ValueFactory::GetDecimalValueFromString("0b.5");
        ASSERT_EQ(0,1);
    }
    catch (...) {
    }

    ASSERT_EQ(1,1);

}

TEST(ValueTest, TestCastToBigInt) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(255);
    Value integer = ValueFactory::GetIntegerValue(243432);
    Value bigInt = ValueFactory::GetBigIntValue(2323325432453);
    Value doubleValue = ValueFactory::GetDoubleValue(244643.1236);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    Value bigIntCastToBigInt = ValueFactory::CastAsBigInt(bigInt);
    EXPECT_EQ(ValuePeeker::PeekBigInt(bigIntCastToBigInt), 2323325432453);

    Value integerCastToBigInt = ValueFactory::CastAsBigInt(integer);
    EXPECT_EQ(ValuePeeker::PeekBigInt(integerCastToBigInt), 243432);

    Value smallIntCastToBigInt = ValueFactory::CastAsBigInt(smallInt);
    EXPECT_EQ(ValuePeeker::PeekBigInt(smallIntCastToBigInt), 255);

    Value tinyIntCastToBigInt = ValueFactory::CastAsBigInt(tinyInt);
    EXPECT_EQ(ValuePeeker::PeekBigInt(tinyIntCastToBigInt), 120);

    Value doubleCastToBigInt = ValueFactory::CastAsBigInt(doubleValue);
    EXPECT_EQ(ValuePeeker::PeekBigInt(doubleCastToBigInt), 244643);

    bool caught = false;
    try
    {
        Value decimalCastToBigInt = ValueFactory::CastAsBigInt(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value stringCastToBigInt = ValueFactory::CastAsBigInt(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Now run a series of tests to make sure that out of range casts fail
    // For BigInt only a double can be out of range.
    Value doubleOutOfRangeH = ValueFactory::GetDoubleValue(92233720368547075809.0);
    Value doubleOutOfRangeL = ValueFactory::GetDoubleValue(-92233720368547075809.0);

    caught = false;
    try
    {
        Value doubleCastToBigInt = ValueFactory::CastAsBigInt(doubleOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value doubleCastToBigInt = ValueFactory::CastAsBigInt(doubleOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToInteger) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(255);
    Value integer = ValueFactory::GetIntegerValue(243432);
    Value bigInt = ValueFactory::GetBigIntValue(232332);
    Value doubleValue = ValueFactory::GetDoubleValue(244643.1236);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    Value bigIntCastToInteger = ValueFactory::CastAsInteger(bigInt);
    EXPECT_EQ(ValuePeeker::PeekInteger(bigIntCastToInteger), 232332);

    Value integerCastToInteger = ValueFactory::CastAsInteger(integer);
    EXPECT_EQ(ValuePeeker::PeekInteger(integerCastToInteger), 243432);

    Value smallIntCastToInteger = ValueFactory::CastAsInteger(smallInt);
    EXPECT_EQ(ValuePeeker::PeekInteger(smallIntCastToInteger), 255);

    Value tinyIntCastToInteger = ValueFactory::CastAsInteger(tinyInt);
    EXPECT_EQ(ValuePeeker::PeekInteger(tinyIntCastToInteger), 120);

    Value doubleCastToInteger = ValueFactory::CastAsInteger(doubleValue);
    EXPECT_EQ(ValuePeeker::PeekInteger(doubleCastToInteger), 244643);

    bool caught = false;
    try
    {
        Value decimalCast = ValueFactory::CastAsInteger(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value stringCast = ValueFactory::CastAsInteger(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Now run a series of tests to make sure that out of range casts fail
    // For Integer only a double and BigInt can be out of range.

    Value doubleOutOfRangeH = ValueFactory::GetDoubleValue(92233720368547075809.0);
    Value doubleOutOfRangeL = ValueFactory::GetDoubleValue(-92233720368547075809.0);
    caught = false;
    try
    {
        Value doubleCastToInteger = ValueFactory::CastAsInteger(doubleOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value doubleCastToInteger = ValueFactory::CastAsInteger(doubleOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value bigIntOutOfRangeH = ValueFactory::GetBigIntValue(4294967297);
    Value bigIntOutOfRangeL = ValueFactory::GetBigIntValue(-4294967297);

    caught = false;
    try
    {
        Value bigIntCastToInteger = ValueFactory::CastAsInteger(bigIntOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value bigIntCastToInteger = ValueFactory::CastAsInteger(bigIntOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToSmallInt) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(255);
    Value integer = ValueFactory::GetIntegerValue(3432);
    Value bigInt = ValueFactory::GetBigIntValue(2332);
    Value doubleValue = ValueFactory::GetDoubleValue(4643.1236);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    Value bigIntCastToSmallInt = ValueFactory::CastAsSmallInt(bigInt);
    EXPECT_EQ(ValuePeeker::PeekSmallInt(bigIntCastToSmallInt), 2332);

    Value integerCastToSmallInt = ValueFactory::CastAsSmallInt(integer);
    EXPECT_EQ(ValuePeeker::PeekSmallInt(integerCastToSmallInt), 3432);

    Value smallIntCastToSmallInt = ValueFactory::CastAsSmallInt(smallInt);
    EXPECT_EQ(ValuePeeker::PeekSmallInt(smallIntCastToSmallInt), 255);

    Value tinyIntCastToSmallInt = ValueFactory::CastAsSmallInt(tinyInt);
    EXPECT_EQ(ValuePeeker::PeekSmallInt(tinyIntCastToSmallInt), 120);

    Value doubleCastToSmallInt = ValueFactory::CastAsSmallInt(doubleValue);
    EXPECT_EQ(ValuePeeker::PeekSmallInt(doubleCastToSmallInt), 4643);

    bool caught = false;
    try
    {
        Value decimalCast = ValueFactory::CastAsSmallInt(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value stringCast = ValueFactory::CastAsSmallInt(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);


    // Now run a series of tests to make sure that out of range casts fail
    // For SmallInt only a double, BigInt, and Integer can be out of range.

    Value doubleOutOfRangeH = ValueFactory::GetDoubleValue(92233720368547075809.0);
    Value doubleOutOfRangeL = ValueFactory::GetDoubleValue(-92233720368547075809.0);
    caught = false;
    try
    {
        Value doubleCastToSmallInt = ValueFactory::CastAsSmallInt(doubleOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value doubleCastToSmallInt = ValueFactory::CastAsSmallInt(doubleOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value bigIntOutOfRangeH = ValueFactory::GetBigIntValue(4294967297);
    Value bigIntOutOfRangeL = ValueFactory::GetBigIntValue(-4294967297);

    caught = false;
    try
    {
        Value bigIntCastToSmallInt = ValueFactory::CastAsSmallInt(bigIntOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value bigIntCastToSmallInt = ValueFactory::CastAsSmallInt(bigIntOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value integerOutOfRangeH = ValueFactory::GetIntegerValue(429496729);
    Value integerOutOfRangeL = ValueFactory::GetIntegerValue(-429496729);

    caught = false;
    try
    {
        Value integerCastToSmallInt = ValueFactory::CastAsSmallInt(integerOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value integerCastToSmallInt = ValueFactory::CastAsSmallInt(integerOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToTinyInt) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(120);
    Value integer = ValueFactory::GetIntegerValue(120);
    Value bigInt = ValueFactory::GetBigIntValue(-64);
    Value doubleValue = ValueFactory::GetDoubleValue(-32);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    Value bigIntCastToTinyInt = ValueFactory::CastAsTinyInt(bigInt);
    EXPECT_EQ(ValuePeeker::PeekTinyInt(bigIntCastToTinyInt), -64);

    Value integerCastToTinyInt = ValueFactory::CastAsTinyInt(integer);
    EXPECT_EQ(ValuePeeker::PeekTinyInt(integerCastToTinyInt), 120);

    Value smallIntCastToTinyInt = ValueFactory::CastAsTinyInt(smallInt);
    EXPECT_EQ(ValuePeeker::PeekTinyInt(smallIntCastToTinyInt), 120);

    Value tinyIntCastToTinyInt = ValueFactory::CastAsTinyInt(tinyInt);
    EXPECT_EQ(ValuePeeker::PeekTinyInt(tinyIntCastToTinyInt), 120);

    Value doubleCastToTinyInt = ValueFactory::CastAsTinyInt(doubleValue);
    EXPECT_EQ(ValuePeeker::PeekTinyInt(doubleCastToTinyInt), -32);

    bool caught = false;
    try
    {
        Value decimalCast = ValueFactory::CastAsTinyInt(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value stringCast = ValueFactory::CastAsTinyInt(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);


    // Now run a series of tests to make sure that out of range casts fail
    // For TinyInt only a double, BigInt, Integer, and SmallInt can be out of range.

    Value doubleOutOfRangeH = ValueFactory::GetDoubleValue(92233720368547075809.0);
    Value doubleOutOfRangeL = ValueFactory::GetDoubleValue(-92233720368547075809.0);
    caught = false;
    try
    {
        Value doubleCastToTinyInt = ValueFactory::CastAsTinyInt(doubleOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value doubleCastToTinyInt = ValueFactory::CastAsTinyInt(doubleOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value bigIntOutOfRangeH = ValueFactory::GetBigIntValue(4294967297);
    Value bigIntOutOfRangeL = ValueFactory::GetBigIntValue(-4294967297);

    caught = false;
    try
    {
        Value bigIntCastToTinyInt = ValueFactory::CastAsTinyInt(bigIntOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value bigIntCastToTinyInt = ValueFactory::CastAsTinyInt(bigIntOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value integerOutOfRangeH = ValueFactory::GetIntegerValue(429496729);
    Value integerOutOfRangeL = ValueFactory::GetIntegerValue(-429496729);

    caught = false;
    try
    {
        Value integerCastToTinyInt = ValueFactory::CastAsTinyInt(integerOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value integerCastToTinyInt = ValueFactory::CastAsTinyInt(integerOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    Value smallIntOutOfRangeH = ValueFactory::GetSmallIntValue(32000);
    Value smallIntOutOfRangeL = ValueFactory::GetSmallIntValue(-3200);

    caught = false;
    try
    {
        Value smallIntCastToTinyInt = ValueFactory::CastAsTinyInt(smallIntOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value smallIntCastToTinyInt = ValueFactory::CastAsTinyInt(smallIntOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToDouble) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(120);
    Value integer = ValueFactory::GetIntegerValue(120);
    Value bigInt = ValueFactory::GetBigIntValue(120);
    Value doubleValue = ValueFactory::GetDoubleValue(120.005);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    Value bigIntCastToDouble = ValueFactory::CastAsDouble(bigInt);
    EXPECT_LT(ValuePeeker::PeekDouble(bigIntCastToDouble), 120.1);
    EXPECT_GT(ValuePeeker::PeekDouble(bigIntCastToDouble), 119.9);

    Value integerCastToDouble = ValueFactory::CastAsDouble(integer);
    EXPECT_LT(ValuePeeker::PeekDouble(integerCastToDouble), 120.1);
    EXPECT_GT(ValuePeeker::PeekDouble(integerCastToDouble), 119.9);

    Value smallIntCastToDouble = ValueFactory::CastAsDouble(smallInt);
    EXPECT_LT(ValuePeeker::PeekDouble(smallIntCastToDouble), 120.1);
    EXPECT_GT(ValuePeeker::PeekDouble(smallIntCastToDouble), 119.9);

    Value tinyIntCastToDouble = ValueFactory::CastAsDouble(tinyInt);
    EXPECT_LT(ValuePeeker::PeekDouble(tinyIntCastToDouble), 120.1);
    EXPECT_GT(ValuePeeker::PeekDouble(tinyIntCastToDouble), 119.9);

    Value doubleCastToDouble = ValueFactory::CastAsDouble(doubleValue);
    EXPECT_LT(ValuePeeker::PeekDouble(doubleCastToDouble), 120.1);
    EXPECT_GT(ValuePeeker::PeekDouble(doubleCastToDouble), 119.9);

    bool caught = false;
    try
    {
        Value decimalCast = ValueFactory::CastAsDouble(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value stringCast = ValueFactory::CastAsDouble(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToString) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(120);
    Value integer = ValueFactory::GetIntegerValue(120);
    Value bigInt = ValueFactory::GetBigIntValue(-64);
    Value doubleValue = ValueFactory::GetDoubleValue(-32);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("10.22");

    bool caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(tinyInt);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(smallInt);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(integer);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(bigInt);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(doubleValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsString(decimalValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();
}

TEST(ValueTest, TestCastToDecimal) {
    Value tinyInt = ValueFactory::GetTinyIntValue(120);
    Value smallInt = ValueFactory::GetSmallIntValue(120);
    Value integer = ValueFactory::GetIntegerValue(120);
    Value bigInt = ValueFactory::GetBigIntValue(120);
    Value doubleValue = ValueFactory::GetDoubleValue(120);
    Value stringValue = ValueFactory::GetStringValue("dude");
    Value decimalValue = ValueFactory::GetDecimalValueFromString("120");

    Value castTinyInt = ValueFactory::CastAsDecimal(tinyInt);
    EXPECT_EQ(0, decimalValue.Compare(castTinyInt));
    Value castSmallInt = ValueFactory::CastAsDecimal(smallInt);
    EXPECT_EQ(0, decimalValue.Compare(castSmallInt));
    Value castInteger = ValueFactory::CastAsDecimal(integer);
    EXPECT_EQ(0, decimalValue.Compare(castInteger));
    Value castBigInt = ValueFactory::CastAsDecimal(bigInt);
    EXPECT_EQ(0, decimalValue.Compare(castBigInt));

    bool caught = false;
    try
    {
        Value cast = ValueFactory::CastAsDecimal(doubleValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value cast = ValueFactory::CastAsDecimal(stringValue);
    }
    catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Make valgrind happy
    stringValue.FreeUninlinedData();

    // Now run a series of tests to make sure that out of range casts fail
    // For Decimal only a double, BigInt, and Integer can be out of range.

    Value doubleOutOfRangeH = ValueFactory::GetDoubleValue(92233720368547075809.0);
    Value doubleOutOfRangeL = ValueFactory::GetDoubleValue(-92233720368547075809.0);
    caught = false;
    try
    {
        Value doubleCastToDecimal = ValueFactory::CastAsDecimal(doubleOutOfRangeH);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try
    {
        Value doubleCastToDecimal = ValueFactory::CastAsDecimal(doubleOutOfRangeL);
    } catch (...)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);
}

// Adding can only overflow BigInt since they are all cast to BigInt before addition takes place.
TEST(ValueTest, TestBigIntOpAddOverflow) {
    Value lhs = ValueFactory::GetBigIntValue(INT64_MAX - 10);
    Value rhs = ValueFactory::GetBigIntValue(INT32_MAX);
    bool caught = false;
    try {
        Value result = lhs.OpAdd(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    lhs = ValueFactory::GetBigIntValue(-(INT64_MAX - 10));
    rhs = ValueFactory::GetBigIntValue(-INT32_MAX);
    caught = false;
    try {
        Value result = lhs.OpAdd(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular addition doesn't throw...
    lhs = ValueFactory::GetBigIntValue(1);
    rhs = ValueFactory::GetBigIntValue(4);
    Value result = lhs.OpAdd(rhs);
}

// Subtraction can only overflow BigInt since they are all cast to BigInt before addition takes place.
TEST(ValueTest, TestBigIntOpSubtractOverflow) {
    Value lhs = ValueFactory::GetBigIntValue(INT64_MAX - 10);
    Value rhs = ValueFactory::GetBigIntValue(-INT32_MAX);
    bool caught = false;
    try {
        Value result = lhs.OpSubtract(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    lhs = ValueFactory::GetBigIntValue(INT64_MAX - 10);
    rhs = ValueFactory::GetBigIntValue(-INT32_MAX);
    caught = false;
    try {
        Value result = lhs.OpSubtract(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular subtraction doesn't throw...
    lhs = ValueFactory::GetBigIntValue(1);
    rhs = ValueFactory::GetBigIntValue(4);
    Value result = lhs.OpSubtract(rhs);
}

// Multiplication can only overflow BigInt since they are all cast to BigInt before addition takes place.
TEST(ValueTest, TestBigIntOpMultiplyOverflow) {
    Value lhs = ValueFactory::GetBigIntValue(INT64_MAX);
    Value rhs = ValueFactory::GetBigIntValue(INT32_MAX);
    bool caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    lhs = ValueFactory::GetBigIntValue(-(INT64_MAX - 10));
    rhs = ValueFactory::GetBigIntValue(INT32_MAX);
    caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    lhs = ValueFactory::GetBigIntValue(INT64_MAX - 10);
    rhs = ValueFactory::GetBigIntValue(-INT32_MAX);
    caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    lhs = ValueFactory::GetBigIntValue(-(INT64_MAX - 10));
    rhs = ValueFactory::GetBigIntValue(-INT32_MAX);
    caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular multiplication doesn't throw...
    lhs = ValueFactory::GetBigIntValue(1);
    rhs = ValueFactory::GetBigIntValue(4);
    Value result = lhs.OpMultiply(rhs);
}

TEST(ValueTest, TestDoubleOpAddOverflow) {
    //Positive infinity
    Value lhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    Value rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    bool caught = false;
    try {
        Value result = lhs.OpAdd(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    //Negative infinity
    lhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).7)));
    rhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).7)));
    caught = false;
    try {
        Value result = lhs.OpAdd(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular addition doesn't throw...
    lhs = ValueFactory::GetDoubleValue(1);
    rhs = ValueFactory::GetDoubleValue(4);
    Value result = lhs.OpAdd(rhs);
}

TEST(ValueTest, TestDoubleOpSubtractOverflow) {
    //Positive infinity
    Value lhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    Value rhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).5)));
    bool caught = false;
    try {
        Value result = lhs.OpSubtract(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    //Negative infinity
    lhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).5)));
    rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    caught = false;
    try {
        Value result = lhs.OpSubtract(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular subtraction doesn't throw...
    lhs = ValueFactory::GetDoubleValue(1.23);
    rhs = ValueFactory::GetDoubleValue(4.2345346);
    Value result = lhs.OpSubtract(rhs);
}

TEST(ValueTest, TestDoubleOpMultiplyOverflow) {
    //Positive infinity
    Value lhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    Value rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    bool caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    //Negative infinity
    lhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).5)));
    rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    caught = false;
    try {
        Value result = lhs.OpMultiply(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular multiplication doesn't throw...
    lhs = ValueFactory::GetDoubleValue(1.23);
    rhs = ValueFactory::GetDoubleValue(4.2345346);
    Value result = lhs.OpMultiply(rhs);
}

TEST(ValueTest, TestDoubleOpDivideOverflow) {
    //Positive infinity
    Value lhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::max());
    Value rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::min());
    bool caught = false;
    try {
        Value result = lhs.OpDivide(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    //Negative infinity
    lhs = ValueFactory::GetDoubleValue(-(std::numeric_limits<double>::max() * ((double).5)));
    rhs = ValueFactory::GetDoubleValue(std::numeric_limits<double>::min());
    caught = false;
    try {
        Value result = lhs.OpDivide(rhs);
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // Sanity check that yes indeed regular division doesn't throw...
    lhs = ValueFactory::GetDoubleValue(1.23);
    rhs = ValueFactory::GetDoubleValue(4.2345346);
    Value result = lhs.OpDivide(rhs);
}

TEST(ValueTest, TestOpIncrementOverflow) {
    Value bigIntValue = ValueFactory::GetBigIntValue(INT64_MAX);
    Value integerValue = ValueFactory::GetIntegerValue(INT32_MAX);
    Value smallIntValue = ValueFactory::GetSmallIntValue(INT16_MAX);
    Value tinyIntValue = ValueFactory::GetTinyIntValue(INT8_MAX);

    bool caught = false;
    try {
        bigIntValue.OpIncrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        integerValue.OpIncrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        smallIntValue.OpIncrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        tinyIntValue.OpIncrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);
}

TEST(ValueTest, TestOpDecrementOverflow) {
    Value bigIntValue = ValueFactory::GetBigIntValue(NSTORE_INT64_MIN);
    Value integerValue = ValueFactory::GetIntegerValue(NSTORE_INT32_MIN);
    Value smallIntValue = ValueFactory::GetSmallIntValue(NSTORE_INT16_MIN);
    Value tinyIntValue = ValueFactory::GetTinyIntValue(NSTORE_INT8_MIN);

    bool caught = false;
    try {
        bigIntValue.OpDecrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        integerValue.OpDecrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        smallIntValue.OpDecrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);

    caught = false;
    try {
        tinyIntValue.OpDecrement();
    } catch (...) {
        caught = true;
    }
    EXPECT_TRUE(caught);
}

TEST(ValueTest, TestComparisonOps)
{
    Value tinyInt = ValueFactory::GetTinyIntValue(101);
    Value smallInt = ValueFactory::GetSmallIntValue(1001);
    Value integer = ValueFactory::GetIntegerValue(1000001);
    Value bigInt = ValueFactory::GetBigIntValue(10000000000001);
    Value floatVal = ValueFactory::GetDoubleValue(12000.456);
    EXPECT_TRUE(smallInt.OpGreaterThan(tinyInt).IsTrue());
    EXPECT_TRUE(integer.OpGreaterThan(smallInt).IsTrue());
    EXPECT_TRUE(bigInt.OpGreaterThan(integer).IsTrue());
    EXPECT_TRUE(tinyInt.OpLessThan(smallInt).IsTrue());
    EXPECT_TRUE(smallInt.OpLessThan(integer).IsTrue());
    EXPECT_TRUE(integer.OpLessThan(bigInt).IsTrue());
    EXPECT_TRUE(tinyInt.OpLessThan(floatVal).IsTrue());
    EXPECT_TRUE(smallInt.OpLessThan(floatVal).IsTrue());
    EXPECT_TRUE(integer.OpGreaterThan(floatVal).IsTrue());
    EXPECT_TRUE(bigInt.OpGreaterThan(floatVal).IsTrue());
    EXPECT_TRUE(floatVal.OpLessThan(bigInt).IsTrue());
    EXPECT_TRUE(floatVal.OpLessThan(integer).IsTrue());
    EXPECT_TRUE(floatVal.OpGreaterThan(smallInt).IsTrue());
    EXPECT_TRUE(floatVal.OpGreaterThan(tinyInt).IsTrue());

    tinyInt = ValueFactory::GetTinyIntValue(-101);
    smallInt = ValueFactory::GetSmallIntValue(-1001);
    integer = ValueFactory::GetIntegerValue(-1000001);
    bigInt = ValueFactory::GetBigIntValue(-10000000000001);
    floatVal = ValueFactory::GetDoubleValue(-12000.456);
    EXPECT_TRUE(smallInt.OpLessThan(tinyInt).IsTrue());
    EXPECT_TRUE(integer.OpLessThan(smallInt).IsTrue());
    EXPECT_TRUE(bigInt.OpLessThan(integer).IsTrue());
    EXPECT_TRUE(tinyInt.OpGreaterThan(smallInt).IsTrue());
    EXPECT_TRUE(smallInt.OpGreaterThan(integer).IsTrue());
    EXPECT_TRUE(integer.OpGreaterThan(bigInt).IsTrue());
    EXPECT_TRUE(tinyInt.OpGreaterThan(floatVal).IsTrue());
    EXPECT_TRUE(smallInt.OpGreaterThan(floatVal).IsTrue());
    EXPECT_TRUE(integer.OpLessThan(floatVal).IsTrue());
    EXPECT_TRUE(bigInt.OpLessThan(floatVal).IsTrue());
    EXPECT_TRUE(floatVal.OpGreaterThan(bigInt).IsTrue());
    EXPECT_TRUE(floatVal.OpGreaterThan(integer).IsTrue());
    EXPECT_TRUE(floatVal.OpLessThan(smallInt).IsTrue());
    EXPECT_TRUE(floatVal.OpLessThan(tinyInt).IsTrue());
}

TEST(ValueTest, TestNullHandling)
{
    Value nullTinyInt = ValueFactory::GetTinyIntValue(INT8_NULL);
    EXPECT_TRUE(nullTinyInt.IsNull());
}

TEST(ValueTest, TestDivideByZero)
{
    Value zeroBigInt = ValueFactory::GetBigIntValue(0);
    Value oneBigInt = ValueFactory::GetBigIntValue(1);
    Value zeroDouble = ValueFactory::GetDoubleValue(0.0);
    Value oneDouble = ValueFactory::GetDoubleValue(1);
    Value oneDecimal = ValueFactory::GetDecimalValueFromString("1");
    Value zeroDecimal = ValueFactory::GetDecimalValueFromString("0");

    Value smallDouble = ValueFactory::GetDoubleValue(DBL_MIN);
    Value smallDecimal = ValueFactory::GetDecimalValueFromString(".000000000001");

    // DECIMAL / DECIMAL
    bool caught_exception = false;
    try
    {
        oneDecimal.OpDivide(zeroDecimal);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // DECIMAL / INT
    caught_exception = false;
    try
    {
        oneDecimal.OpDivide(zeroBigInt);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // INT / DECIMAL
    caught_exception = false;
    try
    {
        oneDecimal.OpDivide(zeroDecimal);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // check result for a really small but non-zero divisor
    caught_exception = false;
    try
    {
        oneDecimal.OpDivide(smallDecimal);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_FALSE(caught_exception);

    // INT / INT
    caught_exception = false;
    try
    {
        oneBigInt.OpDivide(zeroBigInt);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // FLOAT / INT
    caught_exception = false;
    try
    {
        oneDouble.OpDivide(zeroBigInt);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // INT / FLOAT
    caught_exception = false;
    try
    {
        oneBigInt.OpDivide(zeroDouble);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // FLOAT / FLOAT
    caught_exception = false;
    try
    {
        oneDouble.OpDivide(zeroDouble);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_TRUE(caught_exception);

    // check result for a really small but non-zero divisor
    caught_exception = false;
    try
    {
        oneDouble.OpDivide(smallDouble);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_FALSE(caught_exception);

    caught_exception = false;
    try
    {
        oneBigInt.OpDivide(smallDouble);
    }
    catch (...)
    {
        caught_exception = true;
    }
    EXPECT_FALSE(caught_exception);
}

TEST(ValueTest, CompareDecimal)
{
    Value intv;
    Value decv;

    // d.comp(int) pos/pos
    intv = ValueFactory::GetTinyIntValue(120);
    decv = ValueFactory::GetDecimalValueFromString("9999");
    ASSERT_EQ(1, decv.Compare(intv));

    intv = ValueFactory::GetTinyIntValue(120);
    decv = ValueFactory::GetDecimalValueFromString("120");
    ASSERT_EQ(0, decv.Compare(intv));

    intv = ValueFactory::GetTinyIntValue(121);
    decv = ValueFactory::GetDecimalValueFromString("120");
    ASSERT_EQ(-1, decv.Compare(intv));

    // d.comp(int) pos/neg
    intv = ValueFactory::GetTinyIntValue(24);
    decv = ValueFactory::GetDecimalValueFromString("-100");
    ASSERT_EQ(-1, decv.Compare(intv));

    // d.comp(int) neg/pos
    intv = ValueFactory::GetTinyIntValue(-24);
    decv = ValueFactory::GetDecimalValueFromString("23");
    ASSERT_EQ(1, decv.Compare(intv));

    // d.comp(int) neg/neg
    intv = ValueFactory::GetTinyIntValue(-120);
    decv = ValueFactory::GetDecimalValueFromString("-9999");
    ASSERT_EQ(-1, decv.Compare(intv));

    intv = ValueFactory::GetTinyIntValue(-120);
    decv = ValueFactory::GetDecimalValueFromString("-120");
    ASSERT_EQ(0, decv.Compare(intv));

    intv = ValueFactory::GetTinyIntValue(-121);
    decv = ValueFactory::GetDecimalValueFromString("-120");
    ASSERT_EQ(1, decv.Compare(intv));

    // Do int.Compare(decimal)

    // d.comp(int) pos/pos
    intv = ValueFactory::GetTinyIntValue(120);
    decv = ValueFactory::GetDecimalValueFromString("9999");
    ASSERT_EQ(-1, intv.Compare(decv));

    intv = ValueFactory::GetTinyIntValue(120);
    decv = ValueFactory::GetDecimalValueFromString("120");
    ASSERT_EQ(0, intv.Compare(decv));

    intv = ValueFactory::GetTinyIntValue(121);
    decv = ValueFactory::GetDecimalValueFromString("120");
    ASSERT_EQ(1, intv.Compare(decv));

    // d.comp(int) pos/neg
    intv = ValueFactory::GetTinyIntValue(24);
    decv = ValueFactory::GetDecimalValueFromString("-100");
    ASSERT_EQ(1, intv.Compare(decv));

    // d.comp(int) neg/pos
    intv = ValueFactory::GetTinyIntValue(-24);
    decv = ValueFactory::GetDecimalValueFromString("23");
    ASSERT_EQ(-1, intv.Compare(decv));

    // d.comp(int) neg/neg
    intv = ValueFactory::GetTinyIntValue(-120);
    decv = ValueFactory::GetDecimalValueFromString("-9999");
    ASSERT_EQ(1, intv.Compare(decv));

    intv = ValueFactory::GetTinyIntValue(-120);
    decv = ValueFactory::GetDecimalValueFromString("-120");
    ASSERT_EQ(0, intv.Compare(decv));

    intv = ValueFactory::GetTinyIntValue(-121);
    decv = ValueFactory::GetDecimalValueFromString("-120");
    ASSERT_EQ(-1, intv.Compare(decv));
}

TEST(ValueTest, AddDecimal)
{
    Value rhs;
    Value lhs;
    Value ans;
    Value sum;

    // add two decimals
    rhs = ValueFactory::GetDecimalValueFromString("100");
    lhs = ValueFactory::GetDecimalValueFromString("200");
    ans = ValueFactory::GetDecimalValueFromString("300");
    sum = lhs.OpAdd(rhs);
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));
    sum = rhs.OpAdd(lhs);
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));

    // add a big int and a decimal
    rhs = ValueFactory::GetBigIntValue(100);
    sum = lhs.OpAdd(rhs);
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));

    //Overflow
    rhs = ValueFactory::GetDecimalValueFromString("9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("111111111"  //10 digits
            "1111111111"   //20 digits
            "111111.1111"   //30 digits
            "11111111");

    bool caughtException = false;
    try {
        lhs.OpAdd(rhs);
    } catch (...) {
        caughtException = true;
    }
    ASSERT_TRUE(caughtException);

    //Underflow
    rhs = ValueFactory::GetDecimalValueFromString("-9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("-111111111"  //10 digits
            "1111111111"   //20 digits
            "111111.1111"   //30 digits
            "11111111");

    caughtException = false;
    try {
        lhs.OpAdd(rhs);
    } catch (...) {
        caughtException = true;
    }
    ASSERT_TRUE(caughtException);
}

TEST(ValueTest, SubtractDecimal)
{
    Value rhs;
    Value lhs;
    Value ans;
    Value sum;

    // Subtract two decimals
    rhs = ValueFactory::GetDecimalValueFromString("100");
    lhs = ValueFactory::GetDecimalValueFromString("200");
    ans = ValueFactory::GetDecimalValueFromString("100");
    sum = lhs.OpSubtract(rhs);
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));
    sum = rhs.OpSubtract(lhs);
    ans = ValueFactory::GetDecimalValueFromString("-100");
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));

    // Subtract a big int and a decimal
    rhs = ValueFactory::GetBigIntValue(100);
    sum = lhs.OpSubtract(rhs);
    ans = ValueFactory::GetDecimalValueFromString("100");
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(sum));
    ASSERT_EQ(0, ans.Compare(sum));

    //Overflow
    rhs = ValueFactory::GetDecimalValueFromString("-9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("111111111"  //10 digits
            "1111111111"   //20 digits
            "111111.1111"   //30 digits
            "11111111");

    bool caughtException = false;
    try {
        lhs.OpSubtract(rhs);
    } catch (...) {
        caughtException = true;
    }
    ASSERT_TRUE(caughtException);

    //Underflow
    rhs = ValueFactory::GetDecimalValueFromString("9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("-111111111"  //10 digits
            "1111111111"   //20 digits
            "111111.1111"   //30 digits
            "11111111");

    caughtException = false;
    try {
        lhs.OpSubtract(rhs);
    } catch (...) {
        caughtException = true;
    }
    ASSERT_TRUE(caughtException);
}

TEST(ValueTest, DecimalProducts)
{
    Value rhs;
    Value lhs;
    Value product;
    Value ans;

    // decimal * int
    rhs = ValueFactory::GetDecimalValueFromString("218772.7686110");
    lhs = ValueFactory::GetBigIntValue((int64_t)2);
    product = rhs.OpMultiply(lhs);
    ans = ValueFactory::GetDecimalValueFromString("437545.537222");
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(product));
    ASSERT_EQ(ValuePeeker::PeekDecimal(product),
              ValuePeeker::PeekDecimal(ans));

    // int * decimal
    product = lhs.OpMultiply(rhs);
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(product));
    ASSERT_EQ(ValuePeeker::PeekDecimal(product),
              ValuePeeker::PeekDecimal(ans));

    // decimal * decimal
    lhs = ValueFactory::GetDecimalValueFromString("2");
    product = rhs.OpMultiply(lhs);
    ans = ValueFactory::GetDecimalValueFromString("437545.537222");
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(product));
    ASSERT_EQ(ValuePeeker::PeekDecimal(product),
              ValuePeeker::PeekDecimal(ans));

    // decimal * (decimal < 1)
    lhs = ValueFactory::GetDecimalValueFromString("0.21");
    product = rhs.OpMultiply(lhs);
    ans = ValueFactory::GetDecimalValueFromString("45942.281408310");
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(product));
    ASSERT_EQ(ValuePeeker::PeekDecimal(product),
              ValuePeeker::PeekDecimal(ans));

    // decimal that must be rescaled
    rhs = ValueFactory::GetDecimalValueFromString("218772.11111111");
    lhs = ValueFactory::GetDecimalValueFromString("2.001");
    product = rhs.OpMultiply(lhs);
    ans = ValueFactory::GetDecimalValueFromString("437762.99433333111");
    //    cout << "\nlhs " << lhs.debug() << endl;
    //    cout << "rhs " << rhs.debug() << endl;
    //    cout << "answer " << ans.debug() << endl;
    //    cout << "sum    " << sum.debug() << endl;

    // can't produce the answer as a double to compare directly
    ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(product));
    ASSERT_EQ(ValuePeeker::PeekDecimal(product),
              ValuePeeker::PeekDecimal(ans));

    //Overflow
    rhs = ValueFactory::GetDecimalValueFromString("9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("2");

    bool caughtException = false;
    try {
        lhs.OpMultiply(rhs);
    } catch (...) {
        caughtException = true;
    }
    //ASSERT_TRUE(caughtException);

    //Underflow
    rhs = ValueFactory::GetDecimalValueFromString("9999999999"  //10 digits
                                       "9999999999"   //20 digits
                                       "999999.9999"   //30 digits
                                       "99999999");
    lhs = ValueFactory::GetDecimalValueFromString("-2");

    caughtException = false;
    try {
        lhs.OpMultiply(rhs);
    } catch (...) {
        caughtException = true;
    }
    //ASSERT_TRUE(caughtException);
}

TEST(ValueTest, DecimalQuotients) {

   Value rhs;
   Value lhs;
   Value quo;
   Value ans;

   rhs = ValueFactory::GetDecimalValueFromString("200");
   lhs = ValueFactory::GetDecimalValueFromString("5");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("40");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("4003");
   lhs = ValueFactory::GetDecimalValueFromString("20");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("200.15");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("10");
   lhs = ValueFactory::GetDecimalValueFromString("3");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("3.333333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   // sql coverage generated this... and it didn't work
   rhs = ValueFactory::GetDecimalValueFromString("284534.796411");
   lhs = ValueFactory::GetDecimalValueFromString("6");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("47422.4660685");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("1");
   lhs = ValueFactory::GetDecimalValueFromString("3000");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("0.000333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("1");
   lhs = ValueFactory::GetDecimalValueFromString("300");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("0.003333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("1");
   lhs = ValueFactory::GetDecimalValueFromString("30");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("0.033333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("1");
   lhs = ValueFactory::GetDecimalValueFromString("-3");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("-0.333333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("-.0001");
   lhs = ValueFactory::GetDecimalValueFromString(".0003");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("-0.333333333333");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));


   rhs = ValueFactory::GetDecimalValueFromString("-.5555");
   lhs = ValueFactory::GetDecimalValueFromString("-.11");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("5.05");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("0.11");
   lhs = ValueFactory::GetDecimalValueFromString("0.55");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("0.2");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   rhs = ValueFactory::GetDecimalValueFromString("0");
   lhs = ValueFactory::GetDecimalValueFromString("0.55");
   quo = rhs.OpDivide(lhs);
   ans = ValueFactory::GetDecimalValueFromString("0");
   ASSERT_EQ(VALUE_TYPE_DECIMAL, ValuePeeker::PeekValueType(quo));
   ASSERT_EQ(ValuePeeker::PeekDecimal(quo),
             ValuePeeker::PeekDecimal(ans));

   try {
       rhs = ValueFactory::GetDecimalValueFromString("1");
       lhs = ValueFactory::GetDecimalValueFromString("0");
       quo = rhs.OpDivide(lhs);
       ASSERT_EQ(0, 1);
   }
   catch (...) {
       ASSERT_EQ(1,1);
   }
}

TEST(ValueTest, SerializeToExport)
{
    // test basic nvalue elt serialization. Note that
    // NULL values and buffer length checking are done
    // before this primitive function.

    Value nv;

    // a plenty-large-buffer(tm)
    char buf[1024];
    ExportSerializeInput sin(buf, 1024);
    ExportSerializeOutput out(buf, 1024);

    // tinyint
    nv = ValueFactory::GetTinyIntValue(-50);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(-50, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetTinyIntValue(0);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(0, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetTinyIntValue(50);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(50, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    // smallint
    nv = ValueFactory::GetSmallIntValue(-128);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(-128, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetSmallIntValue(0);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(0, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetSmallIntValue(128);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(128, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    // int
    nv = ValueFactory::GetIntegerValue(-4999999);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(-4999999, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetIntegerValue(0);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(0, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetIntegerValue(128);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(128, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    // bigint
    nv = ValueFactory::GetBigIntValue(-4999999);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(-4999999, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetBigIntValue(0);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(0, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetBigIntValue(128);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(128, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    // timestamp
    nv = ValueFactory::GetTimestampValue(99999999);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(99999999, sin.ReadLong());
    sin.Unread(out.Position());
    out.Position(0);

    // double
    nv = ValueFactory::GetDoubleValue(-5.5555);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(-5.5555, sin.ReadDouble());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetDoubleValue(0.0);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(0.0, sin.ReadDouble());
    sin.Unread(out.Position());
    out.Position(0);

    nv = ValueFactory::GetDoubleValue(128.256);
    nv.SerializeToExport(out);
    EXPECT_EQ(8, out.Position());
    EXPECT_EQ(128.256, sin.ReadDouble());
    sin.Unread(out.Position());
    out.Position(0);

    // varchar
    nv = ValueFactory::GetStringValue("ABCDEFabcdef");
    nv.SerializeToExport(out);
    nv.FreeUninlinedData();
    EXPECT_EQ(12 + 4, out.Position());         // chardata plus prefix
    EXPECT_EQ(12, sin.ReadInt()); // 32 bit length prefix
    EXPECT_EQ('A', sin.ReadChar());
    EXPECT_EQ('B', sin.ReadChar());
    EXPECT_EQ('C', sin.ReadChar());
    EXPECT_EQ('D', sin.ReadChar());
    EXPECT_EQ('E', sin.ReadChar());
    EXPECT_EQ('F', sin.ReadChar());
    EXPECT_EQ('a', sin.ReadChar());
    EXPECT_EQ('b', sin.ReadChar());
    EXPECT_EQ('c', sin.ReadChar());
    EXPECT_EQ('d', sin.ReadChar());
    EXPECT_EQ('e', sin.ReadChar());
    EXPECT_EQ('f', sin.ReadChar());
    sin.Unread(out.Position());
    out.Position(0);

    // decimal
    nv = ValueFactory::GetDecimalValueFromString("-1234567890.456123000000");
    nv.SerializeToExport(out);
    EXPECT_EQ(24 + 4, out.Position());
    EXPECT_EQ(24, sin.ReadInt()); // 32 bit length prefix
    EXPECT_EQ('-', sin.ReadChar());
    EXPECT_EQ('1', sin.ReadChar());
    EXPECT_EQ('2', sin.ReadChar());
    EXPECT_EQ('3', sin.ReadChar());
    EXPECT_EQ('4', sin.ReadChar());
    EXPECT_EQ('5', sin.ReadChar());
    EXPECT_EQ('6', sin.ReadChar());
    EXPECT_EQ('7', sin.ReadChar());
    EXPECT_EQ('8', sin.ReadChar());
    EXPECT_EQ('9', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('.', sin.ReadChar());
    EXPECT_EQ('4', sin.ReadChar());
    EXPECT_EQ('5', sin.ReadChar());
    EXPECT_EQ('6', sin.ReadChar());
    EXPECT_EQ('1', sin.ReadChar());
    EXPECT_EQ('2', sin.ReadChar());
    EXPECT_EQ('3', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    EXPECT_EQ('0', sin.ReadChar());
    sin.Unread(out.Position());
    out.Position(0);
}

} // End test namespace
} // End nstore namespace

