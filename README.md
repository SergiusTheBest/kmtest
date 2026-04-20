# KmTest
Kernel-mode (Windows) and user-mode (Windows, Linux) C++ unit testing framework in BDD-style [![CI](https://github.com/SergiusTheBest/kmtest/actions/workflows/ci.yml/badge.svg)](https://github.com/SergiusTheBest/kmtest/actions/workflows/ci.yml)

- [Introduction](#introduction)
  - [Features](#features)
  - [Requirements](#requirements)
- [Usage](#usage)
  - [Integration](#integration)
    - [Using with CMake](#using-with-cmake)
    - [Visual Studio driver project (without CMake)](#visual-studio-driver-project-without-cmake)
  - [Entry points](#entry-points)
    - [User-mode (Windows, Linux)](#user-mode-windows-linux)
    - [Kernel-mode (Windows)](#kernel-mode-windows)
    - [Accessing driver context](#accessing-driver-context)
  - [Writing tests](#writing-tests)
    - [BDD-style test](#bdd-style-test)
      - [Code sharing](#code-sharing)
    - [Traditional test](#traditional-test)
    - [Require clauses](#require-clauses)
  - [Running tests](#running-tests)
    - [Test output](#test-output)
    - [Global Preparation and Cleanup Routines](#global-preparation-and-cleanup-routines)
      - [Global Preparation with `KMTEST_PRE_RUN_ROUTINE`](#global-preparation-with-kmtest_pre_run_routine)
      - [Global Cleanup with `KMTEST_POST_RUN_ROUTINE`](#global-cleanup-with-kmtest_post_run_routine)
- [Samples](#samples)
- [License](#license)
- [Acknowledgment](#acknowledgment)
- [Version history](#version-history)

# Introduction
There is a lack of unit testing frameworks that work in the OS kernel. This library closes that gap and supports both:
- kernel mode on Windows (driver testing)
- user mode on Windows and Linux

Keep one framework, one test style, and shared test code across both user-mode and kernel-mode targets.

## Features
- designed for testing kernel-mode code on Windows
- can run in user mode on Windows and Linux
- CMake-based project with straightforward CMake integration
- one framework for both user-mode and kernel-mode tests
- header-only
- easy to use
- BDD-style approach for writing unit tests (as well as a traditional one)
- code sharing between steps in scenario

## Requirements
- CMake 3.11 and higher
- C++17 compatible compiler:
  - Visual Studio 2017 and higher (msvc)
  - gcc 7.0 and higher
  - clang 4.0 and higher

For Windows kernel-mode driver tests, WDK is required.

# Usage

## Integration

### Using with CMake

KmTest provides a CMake interface target for easy integration with CMake-based projects using FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
  kmtest
  GIT_REPOSITORY https://github.com/SergiusTheBest/kmtest.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(kmtest)
target_link_libraries(your_target kmtest::kmtest)
```

#### CMake options

- `KMTEST_BUILD_SAMPLES` (default: ON for top-level builds): build the sample projects.
- `KMTEST_INSTALL` (default: ON for top-level builds): generate install/export targets.
- `KMTEST_BUILD_TESTS` (default: OFF): build internal tests (if present).

### Visual Studio driver project (without CMake)

1. Create a WDK driver project in Visual Studio.
2. Add the kmtest include directory to Additional Include Directories.
3. In test source files, include `<kmtest/kmtest.h>` (`ntddk.h` is included automatically in kernel mode).
4. Write tests using `SCENARIO`, `GIVEN`, `WHEN`, `THEN`, and `REQUIRE`.

## Entry points

### User-mode (Windows, Linux)

Define `main` in exactly one translation unit using `KMTEST_MAIN()`:

```cpp
#include <kmtest/kmtest.h>

KMTEST_MAIN();
```

### Kernel-mode (Windows)

Include the framework header; `ntddk.h` is included automatically in kernel mode:

```cpp
#include <kmtest/kmtest.h>
```

The framework provides `DriverEntry` automatically; do not define it manually.

### Accessing driver context

In kernel mode, a driver object and registry path are available via:
- `kmtest::g_driverObject`
- `kmtest::g_registryPath`

## Writing tests
You can write test cases in 2 styles:
- BDD-style (using GIVEN-WHEN-THEN clauses)
- traditional

### BDD-style test
BDD-style tests require more effort to write, but they are often easier to maintain than traditional tests. The basic test structure is shown below (for more advanced usage read about [code sharing](#code-sharing)):
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
A great feature of BDD-style tests is that a `SCENARIO` can have several `GIVEN` clauses, a `GIVEN` can have several `WHEN` clauses, and a `WHEN` can have several `THEN` clauses. KmTest runs all combinations as independent test cases. The sample below produces 2 test cases (`2+3=5` and `2+0=2`):
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
### User mode (Windows, Linux)
Run the produced test executable, for example `CalcTest`.

### Kernel mode (Windows)
Running kernel-mode tests means starting a driver. It is highly recommended to do this inside a virtual machine. Any assertion failure will trigger a kernel debugger breakpoint or a BSOD if there is no debugger.

*Refer to [samples/CalcTest/CalcTestDriver.cmd](samples/CalcTest/CalcTestDriver.cmd) for how to start a driver from the command line.*

### Test output
KmTest writes messages to the debug output. In kernel mode, you can view them in WinDbg, DbgView, or similar tools. In user mode, output is printed to standard output.

A shortened sample output is shown below:

```
*******************************************************
* KMTEST BEGIN
*******************************************************
-------------------------------------------------------
SCENARIO: Addition operation
-------------------------------------------------------
GIVEN: x = 2
  WHEN: y = 3
    THEN: the sum will be 5
  WHEN: y = 0
    THEN: the sum will be 2

PASSED (assertions: 2)

*******************************************************
* KMTEST PASSED (scenarios: 3, assertions: 14)
*******************************************************
```

### Global Preparation and Cleanup Routines

You can define the following optional macros to run one-time setup/teardown code:
- `KMTEST_PRE_RUN_ROUTINE`
- `KMTEST_POST_RUN_ROUTINE`

#### Global Preparation with `KMTEST_PRE_RUN_ROUTINE`

If you need to perform some global preparation before any test runs (for example, initializing shared resources or setting up an environment), you can define a function named `KMTEST_PRE_RUN_ROUTINE` with no parameters. This function will be called once before all tests are executed. Use this for setup tasks that should only happen once per test session.

**Example:**
```cpp
#define KMTEST_PRE_RUN_ROUTINE InitializeMyDriverEnvironment

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
#define KMTEST_POST_RUN_ROUTINE ReleaseMyDriverEnvironment

void ReleaseMyDriverEnvironment()
{
    // Cleanup code here
}
```
- The function must have no parameters.
- This routine runs once after all test cases.
- Use it for teardown steps that are expensive or should only happen once per test session.

# Samples
There is a [samples](samples) folder that demonstrates usage of the KmTest unit testing framework.

- `samples/CalcLib`: tiny calculator library used by sample tests.
- `samples/CalcTest`: user-mode test executable (`CalcTest`) on Windows and Linux.
- `samples/CalcTest`: kernel-mode driver test (`CalcTestDriver`) on Windows when WDK 10 is available.

# License
KmTest is licensed under the [MPL version 2.0](http://mozilla.org/MPL/2.0/). You can freely use it in your commercial or open-source software.

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
