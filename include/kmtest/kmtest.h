#pragma once

#define KMTEST_CAT2(x, y)        x##y
#define KMTEST_CAT(x, y)         KMTEST_CAT2(x, y)

#define KMTEST_MAKE_ID(prefix)   KMTEST_CAT(prefix, __LINE__)

#ifdef _KERNEL_MODE
    #define KMTEST_PRINT DbgPrint
    #define KMTEST_ASSERT ::RtlAssert
#else
    #define KMTEST_PRINT printf
    #define KMTEST_ASSERT
#endif

#define SCENARIO(name) \
    namespace kmtest \
    { \
        static void KMTEST_MAKE_ID(testFunc)(Clause curClause, Clause& nextClause, int& assertions, int& failures, bool nextClauseSet = false); \
        static void KMTEST_MAKE_ID(testFuncStub)(Clause curClause, Clause& nextClause, int& assertions, int& failures) \
        { \
            if (curClause == Clause()) reportScenarioBegin(name); \
            KMTEST_MAKE_ID(testFunc)(curClause, nextClause, assertions, failures); \
        } \
        namespace \
        { \
            const TestFunc KMTEST_MAKE_ID(testEntry) = KMTEST_MAKE_ID(testFuncStub);  \
            __declspec(dllexport) __declspec(allocate("KMTEST$__m")) auto KMTEST_MAKE_ID(testEntryPtr) = reinterpret_cast<const TestEntry*>(&KMTEST_MAKE_ID(testEntry)); \
        } \
    } \
    __pragma(warning(suppress: 4100 /*unreferenced formal parameter*/)) \
    static void KMTEST_MAKE_ID(kmtest::testFunc)(Clause curClause, Clause& nextClause, int& assertions, int& failures, bool nextClauseSet)

#define GIVEN(desc) \
    if (curClause.given == 0) curClause.given = __LINE__; \
    if (curClause.given < __LINE__ && !nextClauseSet) { nextClause.given = __LINE__; nextClause.when = nextClause.then = 0; nextClauseSet = true; } \
    else if (__LINE__ == curClause.given && reportGiven(desc))

#define WHEN(desc) \
    if (curClause.when == 0) curClause.when = __LINE__; \
    if (curClause.when < __LINE__ && !nextClauseSet) { nextClause.when = __LINE__; nextClause.then = 0; nextClauseSet = true; } \
    else if (__LINE__ == curClause.when && reportWhen(desc))

#define THEN(desc) \
    if (curClause.then == 0) curClause.then = __LINE__; \
    if (curClause.then < __LINE__ && !nextClauseSet) { nextClause.then = __LINE__; nextClauseSet = true; } \
    else if (__LINE__ == curClause.then && reportThen(desc))

#define REQUIRE(expression) \
    ++assertions; \
    if (!(expression)) \
    { \
        KMTEST_ASSERT(const_cast<char*>(#expression), const_cast<char*>(__FILE__), __LINE__, nullptr); \
        ++failures; \
    }

#define REQUIRE_NT_SUCCESS(expression) \
    REQUIRE(NT_SUCCESS(expression))

#define REQUIRE_NT_FAILURE(expression) \
    REQUIRE(!NT_SUCCESS(expression))

#pragma section("KMTEST$__a", read)
#pragma section("KMTEST$__m", read)
#pragma section("KMTEST$__z", read)
#pragma comment(linker, "/merge:KMTEST=.rdata")

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

    __declspec(selectany) __declspec(allocate("KMTEST$__a")) const TestEntry* testEntryA = nullptr;
    __declspec(selectany) __declspec(allocate("KMTEST$__z")) const TestEntry* testEntryZ = nullptr;

    inline int run()
    {
        KMTEST_PRINT("*******************************************************\n");
        KMTEST_PRINT("* KMTEST BEGIN\n");
        KMTEST_PRINT("*******************************************************\n");

        int scenarios = 0;
        int assertions = 0;
        int failures = 0;

        for (const TestEntry** testEntry = &testEntryA; testEntry < &testEntryZ; ++testEntry)
        {
            if (!*testEntry)
            {
                continue;
            }

            (*testEntry)->run(assertions, failures);
            ++scenarios;
        }

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

    __declspec(selectany) DRIVER_OBJECT* g_driverObject = nullptr;
    __declspec(selectany) UNICODE_STRING g_registryPath = {};
}

#ifdef _KERNEL_MODE
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
int main()
{
    return kmtest::run();
}
#endif
