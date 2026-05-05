#include "pch.h"
#include "tester.h"
#include "gtest/gtest.h"

namespace cvtest::tester
{
    TEST_F(Tester, TestOne)
    {
        EXPECT_EQ(1, 1);
    }
}