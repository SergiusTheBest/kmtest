#include "StdAfx.h"
#include "Calculator.h"

SCENARIO("Addition operation")
{
    GIVEN("x = 2")
    {
        int x = 2;

        WHEN("y = 3")
        {
            int y = 3;

            THEN("the sum will be 5")
            {
                REQUIRE(Calculator::add(x, y) == 5);
            }
        }

        WHEN("y = 0")
        {
            int y = 0;

            THEN("the sum will be 2")
            {
                REQUIRE(Calculator::add(x, y) == 2);
            }
        }

        WHEN("y = -2")
        {
            int y = -2;

            THEN("the sum will be 0")
            {
                REQUIRE(Calculator::add(x, y) == 0);
            }
        }
    }

    GIVEN("x = -2")
    {
        int x = -2;

        WHEN("y = 3")
        {
            int y = 3;

            THEN("the sum will be 1")
            {
                REQUIRE(Calculator::add(x, y) == 1);
            }
        }

        WHEN("y = -1")
        {
            int y = -1;

            THEN("the sum will be -3")
            {
                REQUIRE(Calculator::add(x, y) == -3);
            }
        }
    }
}