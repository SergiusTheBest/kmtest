#include <kmtest/kmtest.h>
#include "Calculator.h"

SCENARIO("Compile time constant")
{
    REQUIRE(12 == 2 * 6); // Make sure that the test framework can handle compile time constants.
}