#include "StdAfx.h"
#include "Calculator.h"

// Just one more sample scenario (very close to "addition operation").
SCENARIO("Subtraction operation")
{
    GIVEN("x = 8")
    {
        int x = 8;

        WHEN("y = 3")
        {
            int y = 3;

            THEN("the difference will be 5")
            {
                REQUIRE(Calculator::sub(x, y) == 5);
            }
        }

        WHEN("y = 0")
        {
            int y = 0;

            THEN("the difference will be 8")
            {
                REQUIRE(Calculator::sub(x, y) == 8);
            }
        }

        WHEN("y = -2")
        {
            int y = -2;

            THEN("the difference will be 10")
            {
                REQUIRE(Calculator::sub(x, y) == 10);
            }
        }
    }

    GIVEN("x = -3")
    {
        int x = -3;

        WHEN("y = 2")
        {
            int y = 2;

            THEN("the difference will be -5")
            {
                REQUIRE(Calculator::sub(x, y) == -5);
            }
        }

        WHEN("y = -1")
        {
            int y = -1;

            THEN("the difference will be -2")
            {
                REQUIRE(Calculator::sub(x, y) == -2);
            }
        }
    }
}