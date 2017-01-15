#include "StdAfx.h"
#include "Calculator.h"

// A minimal scenario for those who do not want to write GIVEN-WHEN-THEN clauses.
SCENARIO("Multiplication operation")
{
    REQUIRE(6 == Calculator::mul(2, 3));
    REQUIRE(-30 == Calculator::mul(-10, 3));
    REQUIRE(6 == Calculator::mul(-2, -3));
    REQUIRE(0 == Calculator::mul(0, 3));
}