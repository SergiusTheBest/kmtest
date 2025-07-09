# KmTest
Kernel-mode C++ unit testing framework in BDD-style [![Build status](https://ci.appveyor.com/api/projects/status/oh90njhd50dfwco0/branch/master?svg=true)](https://ci.appveyor.com/project/SergiusTheBest/kmtest/branch/master)

- [Introduction](#introduction)
  - [Features](#features)
  - [Requirements](#requirements)
- [Usage](#usage)
  - [Creating a test project](#creating-a-test-project)
  - [Main function](#main-function)
  - [Writing tests](#writing-tests)
    - [BDD-style test](#bdd-style-test)
    - [Traditional test](#traditional-test)
    - [Require clauses](#require-clauses)
  - [Running tests](#running-tests)
    - [Test output](#test-output)
- [Samples](#samples)
- [License](#license)
- [Acknowledgment](#acknowledgment)
- [Version history](#version-history)

# Introduction
There is a lack of unit testing frameworks that work in OS kernel. This library closes that gap and is targeted for Windows driver developers.

## Features
- designed for testing kernel-mode code
- can run in user mode (for testing mode-independent code)
- header-only
- easy to use
- BDD-style approach for writing unit tests (as well as a traditional one)
- code sharing between steps in scenario

## Requirements
- Windows XP and higher
- Visual Studio 2010 and higher

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

## Main function
`DriverEntry` or `main` is automatically created by the library, so you don't need to write it. 

A driver object and a registry path can be accessed via `kmtest::g_driverObject` and `kmtest::g_registryPath`.


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

### Require clauses
Requires clauses are used for assertions. There are several of them:
|Clause|Expression return type|Expression expected value|
|--|--|--|
|REQUIRE(expression)|bool|true|
|REQUIRE_NT_SUCCESS(expression)|NTSTATUS|NT_SUCCESS(status)|
|REQUIRE_NT_FAILURE(expression)|NTSTATUS|!NT_SUCCESS(status)|

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

### Global Preparation and Cleanup Routines

KMTEST_PRE_RUN_ROUTINE
KMTEST_POST_RUN_ROUTINE

#### Global Preparation with `KMTEST_PRE_RUN_ROUTINE`

If you need to perform some global preparation before any test runs (for example, initializing shared resources or setting up an environment), you can define a function named `KMTEST_PRE_RUN_ROUTINE` with no parameters. This function will be called once before all tests are executed. Use this for setup tasks that should only happen once per test session.

**Example:**
```cpp
#define KMTEST_PRE_RUN_ROUTINE() InitializeMyDriverEnvironment()

void InitializeMyDriverEnvironment()
{
    // Initialization code here
}

```
- The function must have no parameters.
- This routine runs once before all test cases.
- Use it for setup steps that are expensive or should only happen once per test session.

#### Global Cleanup with `KMTEST_POST_RUN_ROUTINE`

If you need to perform cleanup after all tests have been executed (for example, releasing shared resources or environment teardown), you can define a function named `KMTEST_POST_RUN_ROUTINE` with no parameters. This function will be called once after all tests have finished running. Use this for cleanup tasks that should only happen once per test session.

**Example:**
```cpp
#define KMTEST_POST_RUN_ROUTINE() ReleaseMyDriverEnvironment()

void ReleaseMyDriverEnvironment()
{
    // Cleanup code here
}
```
- The function must have no parameters.
- This routine runs once after all test cases.
- Use it for teardown steps that are expensive or should only happen once per test session.

# Samples
There is a [samples](samples) folder that demonstrates usage of KmTest unit testing framework. To compile it you need Visual Studio 2015 and WDK10.

# License
KmTest is licensed under the [MPL version 2.0](http://mozilla.org/MPL/2.0/). You can freely use it in your commercial or opensource software.

# Acknowledgment
Thanks to Phil Nash and his [Catch C++ test framework](https://github.com/philsquared/Catch) for BDD-style inspiration.

# Version history

## Version 0.9.1 (26 May 2022)
- New: Add ability to run in user space #2
- New: Save `DriverEntry` arguments #4
- New: Add macro REQUIRE_SUCCESS and REQUIRE_FAILURE #5
- Fix: Cannot convert `const char []` to `PVOID` #6

## Version 0.9.0 (18 Jan 2017)
- Initial public release
