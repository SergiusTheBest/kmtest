# KmTest
Kernel-mode C++ unit testing framework in BDD-style [![Build status](https://ci.appveyor.com/api/projects/status/oh90njhd50dfwco0/branch/master?svg=true)](https://ci.appveyor.com/project/SergiusTheBest/kmtest/branch/master)

- [Introduction](#introduction)
  - [Features](#features)
  - [Requirements](#requirements)
- [Usage](#usage)
  - [Creating a test project](#creating-a-test-project)
  - [Writing tests](#writing-tests)
    - [BDD-style test](#bdd-style-test)
    - [Traditional test](#traditional-test)
  - [Running tests](#running-tests)
    - [Test output](#test-output)
- [Samples](#samples)
- [License](#license)
- [Version history](#version-history)

# Introduction
There is a lack of unit testing frameworks that work in OS kernel. This library closes that gap.

## Features
- designed for testing kernel-mode code
- header-only
- easy to use
- BDD-style approach for writing unit tests (as well as a traditional one)
- code sharing between steps in scenario

## Requirements
- Windows XP and higher
- Visual Studio 2010 and higher (probably older Visual Studio will work too)

# Usage

## Creating a test project
Create an empty driver project and do the following:
- add a path to `kmtest/inlcude` into the project include paths
- add `#include <kmtest/kmtest.h>` into your new cpp/h files (if you have precompiled headers it is a good place to add this include there)

This is a sample precompiled header:
```cpp
#pragma once
#include <ntddk.h>
#include <kmtest/kmtest.h>
```

Now you can start writing tests.

*Note: `DriverEntry` is automatically created by the library, so you don't need to write it.*

## Writing tests
You can write tests cases in 2 styles:
- BDD-style (using GIVEN-WHEN-THEN clauses)
- traditional

### BDD-style test
BDD-style tests requires more efforts in writing but they are superior in maintaining than traditional tests. The basic test structure is shown below (for more advanced usage read about [code sharing](#code-sharing)):
```cpp
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
    }
}
```
Where:
- `SCENARIO`,`GIVEN`,`WHEN`,`THEN` are used to describe the test
- `REQUIRE` is used for assertions (can be placed in any block)

#### Code sharing
A great feature of BDD-style tests is that a `SCENARIO` can have several `GIVEN` clauses, a `GIVEN` can have several `WHEN` clauses, a `WHEN` can have several `THEN` clauses. KmTest framework will run all combinations as independed test cases. The sample below will produce 2 test cases (`2+3=5` and `2+0=2`):
```cpp
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
    }
}    
```
That's not all. Setup/cleanup code can be shared as well. It is demonstrated by the following example:
```cpp
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
                REQUIRE(Calculator::add(x, y) == 5);
            }

            // <== Here you can write a shared cleanup code for WHEN.
        }

        // <== Here you can write a shared cleanup code for GIVEN.
    }

    // <== Here you can write a shared cleanup code for SCENARIO.
}
```

### Traditional test
A traditional test is shown below and is represented by a BDD-style test without GIVEN-WHEN-THEN clauses:
```cpp
// A minimal scenario for those who do not want to write GIVEN-WHEN-THEN clauses.
SCENARIO("Multiplication operation")
{
    REQUIRE(6 == Calculator::mul(2, 3));
    REQUIRE(-30 == Calculator::mul(-10, 3));
    REQUIRE(6 == Calculator::mul(-2, -3));
    REQUIRE(0 == Calculator::mul(0, 3));
}
```
Where:
- `SCENARIO` is used to describe the test
- `REQUIRE` is used for assertions

## Running tests
Running KmTest based tests means starting a driver. It is highly recommended to do this inside a virtual machine. Any assertion failure will trigger a kernel debugger breakpoint or a BSOD if there is no debugger.

*Refer to [samples/CalcTest/CalcTest.cmd](samples/CalcTest/CalcTest.cmd) for how to start a driver from the command line.*

### Test output
KmTest writes messages to the debug output. It can be viewed by WinDbg, DbgView or similar tools. A sample test output is demonstrated below:

```
**************************************************
* KMTEST BEGIN
**************************************************
--------------------------------------------------
SCENARIO: Addition operation
--------------------------------------------------
GIVEN: x = 2
  WHEN: y = 3
    THEN: the sum will be 5
GIVEN: x = 2
  WHEN: y = 0
    THEN: the sum will be 2
GIVEN: x = 2
  WHEN: y = -2
    THEN: the sum will be 0
GIVEN: x = -2
  WHEN: y = 3
    THEN: the sum will be 1
GIVEN: x = -2
  WHEN: y = -1
    THEN: the sum will be -3
 
ASSERTIONS PASSED: 5
 
--------------------------------------------------
SCENARIO: Multiplication operation
--------------------------------------------------
 
ASSERTIONS PASSED: 4
 
--------------------------------------------------
SCENARIO: Subtraction operation
--------------------------------------------------
GIVEN: x = 8
  WHEN: y = 3
    THEN: the difference will be 5
GIVEN: x = 8
  WHEN: y = 0
    THEN: the difference will be 8
GIVEN: x = 8
  WHEN: y = -2
    THEN: the difference will be 10
GIVEN: x = -3
  WHEN: y = 2
    THEN: the difference will be -5
GIVEN: x = -3
  WHEN: y = -1
    THEN: the difference will be -2
 
ASSERTIONS PASSED: 5
 
**************************************************
* KMTEST END (scenarios: 3, assertions: 14)
**************************************************
```

# Samples
There is a [samples](samples) folder that demonstrates usage of KmTest unit testing framework. To compile it you need Visual Studio 2015 and WDK10.

# License
KmTest is licensed under the [MPL version 2.0](http://mozilla.org/MPL/2.0/). You can freely use it in your commercial or opensource software.

# Version history

## Version 0.6.0 (16 Jan 2017)
- Initial public release
