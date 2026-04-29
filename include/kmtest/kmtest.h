#pragma once

#ifndef _KERNEL_MODE
    #include <stdio.h>
#else
    #include <ntddk.h>
#endif

#if defined(_MSC_VER)
    #define KMTEST_SECTION(name)                __declspec(dllexport) __declspec(allocate(name))
    #define KMTEST_SECTION_START(type, var)     KMTEST_SECTION("KMTEST$__a") inline const type* const var = nullptr
    #define KMTEST_SECTION_MIDDLE               KMTEST_SECTION("KMTEST$__m")
    #define KMTEST_SECTION_END(type, var)       KMTEST_SECTION("KMTEST$__z") inline const type* const var = nullptr
    #pragma section("KMTEST$__a", read)
    #pragma section("KMTEST$__m", read)
    #pragma section("KMTEST$__z", read)
    #pragma comment(linker, "/merge:KMTEST=.rdata")
    #define KMTEST_SUPPRESS_CONDITIONAL_EXPRESSION_IS_CONSTANT()    __pragma(warning(suppress: 4127 /*conditional expression is constant*/))
#elif defined(__GNUC__) || defined(__clang__)
    #define KMTEST_SECTION(name)                __attribute__((section(name), used))
    #define KMTEST_SECTION_START(type, var)     extern "C" const type* const __start_KMTEST; inline const type* const& var = __start_KMTEST;
    #define KMTEST_SECTION_MIDDLE               KMTEST_SECTION("KMTEST")
    #define KMTEST_SECTION_END(type, var)       extern "C" const type* const __stop_KMTEST; inline const type* const& var = __stop_KMTEST;
    #define KMTEST_SUPPRESS_CONDITIONAL_EXPRESSION_IS_CONSTANT()
#else
    #error "Unknown compiler"
#endif

#define KMTEST_IMPL_CAT(x, y)   x##y
#define KMTEST_CAT(x, y)        KMTEST_IMPL_CAT(x, y)

#ifdef _KERNEL_MODE
    #define KMTEST_PRINT DbgPrint
    #define KMTEST_ASSERT ::RtlAssert
#else
    #define KMTEST_PRINT printf
    #define KMTEST_ASSERT(expression, file, line, dummy) fprintf(stderr, "ASSERTION FAILED: %s\nFILE: %s\nLINE: %d\n", expression, file, line)
#endif

#define SCENARIO(name) KMTEST_IMPL_SCENARIO(name, __COUNTER__)

#define KMTEST_IMPL_SCENARIO(name, counter) \
    namespace kmtest \
    { \
        static void KMTEST_CAT(testFunc, counter)([[maybe_unused]] Clause curClause, [[maybe_unused]] Clause& nextClause, [[maybe_unused]] int& assertions, [[maybe_unused]] int& failures, [[maybe_unused]] bool nextClauseSet = false); \
        static void KMTEST_CAT(testFuncStub, counter)([[maybe_unused]] Clause curClause, [[maybe_unused]] Clause& nextClause, [[maybe_unused]] int& assertions, [[maybe_unused]] int& failures) \
        { \
            if (curClause == Clause()) reportScenarioBegin(name); \
            KMTEST_CAT(testFunc, counter)(curClause, nextClause, assertions, failures); \
        } \
        namespace \
        { \
            const TestFunc KMTEST_CAT(testEntry, counter) = KMTEST_CAT(testFuncStub, counter);  \
            KMTEST_SECTION_MIDDLE [[maybe_unused]] auto KMTEST_CAT(testEntryPtr, counter) = reinterpret_cast<const TestEntry*>(&KMTEST_CAT(testEntry, counter)); \
        } \
    } \
    static void KMTEST_CAT(kmtest::testFunc, counter)([[maybe_unused]] Clause curClause, [[maybe_unused]] Clause& nextClause, [[maybe_unused]] int& assertions, [[maybe_unused]] int& failures, [[maybe_unused]] bool nextClauseSet)

#define GIVEN(desc) KMTEST_IMPL_GIVEN(desc, __COUNTER__)

#define KMTEST_IMPL_GIVEN(desc, counter) \
    if (curClause.given == 0) curClause.given = counter; \
    if (curClause.given < counter && !nextClauseSet) { nextClause.given = counter; nextClause.when = nextClause.then = 0; nextClauseSet = true; } \
    else if (counter == curClause.given && reportGiven(desc))

#define WHEN(desc) KMTEST_IMPL_WHEN(desc, __COUNTER__)

#define KMTEST_IMPL_WHEN(desc, counter) \
    if (curClause.when == 0) curClause.when = counter; \
    if (curClause.when < counter && !nextClauseSet) { nextClause.when = counter; nextClause.then = 0; nextClauseSet = true; } \
    else if (counter == curClause.when && reportWhen(desc))

#define THEN(desc) KMTEST_IMPL_THEN(desc, __COUNTER__)

#define KMTEST_IMPL_THEN(desc, counter) \
    if (curClause.then == 0) curClause.then = counter; \
    if (curClause.then < counter && !nextClauseSet) { nextClause.then = counter; nextClauseSet = true; } \
    else if (counter == curClause.then && reportThen(desc))

    #define REQUIRE(expression) \
    ++assertions; \
    KMTEST_SUPPRESS_CONDITIONAL_EXPRESSION_IS_CONSTANT() \
    if (!(expression)) \
    { \
        KMTEST_ASSERT(const_cast<char*>(#expression), const_cast<char*>(__FILE__), __LINE__, nullptr); \
        ++failures; \
    }

#define REQUIRE_NT_SUCCESS(expression) \
    REQUIRE(NT_SUCCESS(expression))

#define REQUIRE_NT_FAILURE(expression) \
    REQUIRE(!NT_SUCCESS(expression))

namespace kmtest
{
    inline void reportScenarioBegin(const char* scenario)
    {
        KMTEST_PRINT("-------------------------------------------------------\n");
        KMTEST_PRINT("SCENARIO: %s\n", scenario);
        KMTEST_PRINT("-------------------------------------------------------\n");
    }

    inline void reportScenarioEnd(int assertions, int failures)
    {
        if (!failures)
        {
            KMTEST_PRINT("\nPASSED (assertions: %d)\n\n", assertions);
        }
        else
        {
            KMTEST_PRINT("\nFAILED (assertions: %d, failures: %d)\n\n", assertions, failures);
        }
    }

    inline bool reportGiven(const char* given)
    {
        KMTEST_PRINT("GIVEN: %s\n", given);
        return true;
    }

    inline bool reportWhen(const char* when)
    {
        KMTEST_PRINT("  WHEN: %s\n", when);
        return true;
    }

    inline bool reportThen(const char* then)
    {
        KMTEST_PRINT("    THEN: %s\n", then);
        return true;
    }

    struct Clause
    {
        int given = 0;
        int when = 0;
        int then = 0;

        bool operator==(const Clause& other) const
        {
            return given == other.given && when == other.when && then == other.then;
        }
    };

    typedef void(*TestFunc)(Clause curClause, Clause& nextClause, int& assertions, int& failures);

    class TestEntry
    {
    public:
        void run(int& assertions, int& failures) const
        {
            Clause curClause = {};
            Clause nextClause = {};
            int localAssertions = 0;
            int localFailures = 0;

            for (;;)
            {
                m_func(curClause, nextClause, localAssertions, localFailures);

                if (nextClause == curClause)
                {
                    break;
                }

                curClause = nextClause;
            }

            reportScenarioEnd(localAssertions, localFailures);
            assertions += localAssertions;
            failures += localFailures;
        }

    private:
        TestEntry();

    private:
        const TestFunc m_func;
    };

    KMTEST_SECTION_START(TestEntry, testEntryA);
    KMTEST_SECTION_END(TestEntry, testEntryZ);

    inline int run()
    {
        KMTEST_PRINT("*******************************************************\n");
        KMTEST_PRINT("* KMTEST BEGIN\n");
        KMTEST_PRINT("*******************************************************\n");

        #ifdef KMTEST_PRE_RUN_ROUTINE
            KMTEST_PRE_RUN_ROUTINE();
        #endif

        int scenarios = 0;
        int assertions = 0;
        int failures = 0;

        for (auto testEntry = &testEntryA; testEntry < &testEntryZ; ++testEntry)
        {
            if (!*testEntry)
            {
                continue;
            }

            (*testEntry)->run(assertions, failures);
            ++scenarios;
        }

        #ifdef KMTEST_POST_RUN_ROUTINE
            KMTEST_POST_RUN_ROUTINE();
        #endif

        KMTEST_PRINT("*******************************************************\n");
        if (!failures)
        {
            KMTEST_PRINT("* KMTEST PASSED (scenarios: %d, assertions: %d)\n", scenarios, assertions);
        }
        else
        {
            KMTEST_PRINT("* KMTEST FAILED (scenarios: %d, assertions: %d, failures: %d)\n", scenarios, assertions, failures);
        }
        KMTEST_PRINT("*******************************************************\n");

        return failures;
    }
}

#ifdef _KERNEL_MODE
namespace kmtest
{
    __declspec(selectany) DRIVER_OBJECT* g_driverObject = nullptr;
    __declspec(selectany) UNICODE_STRING g_registryPath = {};
}

DRIVER_UNLOAD DriverUnload;
inline void DriverUnload(_In_ DRIVER_OBJECT*)
{
}

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" inline NTSTATUS DriverEntry(_In_ DRIVER_OBJECT* driverObject, _In_ PUNICODE_STRING registryPath)
{
    kmtest::g_driverObject = driverObject;
    kmtest::g_registryPath = *registryPath;

    kmtest::run();

    driverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}
#else
#define KMTEST_MAIN() int main() { return kmtest::run(); }
#endif
