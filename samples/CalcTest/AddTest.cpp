#include "StdAfx.h"
#include "Calculator.h"

// A demo scenario that shows the most of the test framework usage.
SCENARIO("Addition operation")
{
    // <== Here you can write a shared setup code for SCENARIO.

    GIVEN("x = 2")
    {
        int x = 2; // <== Here you can write a shared setup code for GIVEN.

        WHEN("y = 3")
        {
            int y = 3; // <== Here you can write a shared setup code for WHEN.

            THEN("the sum will be 5")
            {
                REQUIRE(Calculator::add(x, y) == 5); // This is a test assertion macro. They can be placed in any clause: SCENARIO, GIVEN, WHEN, THEN.
            }

            // <== Here you can write a shared cleanup code for WHEN.
        }

        WHEN("y = 0") // A GIVEN can have several WHEN clauses.
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

        // <== Here you can write a shared cleanup code for GIVEN.
    }

    GIVEN("x = -2") // A SCENARIO can have several GIVEN clauses.
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

    // <== Here you can write a shared cleanup code for SCENARIO.
}