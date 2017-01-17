#pragma once

#define KMTEST_CAT2(x, y)        x##y
#define KMTEST_CAT(x, y)         KMTEST_CAT2(x, y)

#define KMTEST_MAKE_ID(prefix)   KMTEST_CAT(prefix, __LINE__)

#define SCENARIO(name) \
    namespace kmtest \
    { \
        static void KMTEST_MAKE_ID(testFunc)(Clause curClause, Clause& nextClause, int& assertions, bool nextClauseSet = false); \
        static void KMTEST_MAKE_ID(testFuncStub)(Clause curClause, Clause& nextClause, int& assertions) \
        { \
            if (curClause == Clause()) reportScenarioBegin(name); \
            KMTEST_MAKE_ID(testFunc)(curClause, nextClause, assertions); \
        } \
        namespace \
        { \
            const TestFunc KMTEST_MAKE_ID(testEntry) = KMTEST_MAKE_ID(testFuncStub);  \
            __declspec(dllexport) __declspec(allocate("KMTEST$__m")) auto KMTEST_MAKE_ID(testEntryPtr) = reinterpret_cast<const TestEntry*>(&KMTEST_MAKE_ID(testEntry)); \
        } \
    } \
    __pragma(warning(suppress: 4100 /*unreferenced formal parameter*/)) \
    static void KMTEST_MAKE_ID(kmtest::testFunc)(Clause curClause, Clause& nextClause, int& assertions, bool nextClauseSet)

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
        ::RtlAssert(#expression, __FILE__, __LINE__, nullptr); \
    }

#pragma section("KMTEST$__a", read)
#pragma section("KMTEST$__m", read)
#pragma section("KMTEST$__z", read)
#pragma comment(linker, "/merge:KMTEST=.rdata")

namespace kmtest
{
    inline void reportScenarioBegin(const char* scenario)
    {
        DbgPrint("--------------------------------------------------\n");
        DbgPrint("SCENARIO: %s\n", scenario);
        DbgPrint("--------------------------------------------------\n");
    }

    inline void reportScenarioEnd(int assertions)
    {
        DbgPrint("\nASSERTIONS PASSED: %d\n\n", assertions);
    }

    inline bool reportGiven(const char* given)
    {
        DbgPrint("GIVEN: %s\n", given);
        return true;
    }

    inline bool reportWhen(const char* when)
    {
        DbgPrint("  WHEN: %s\n", when);
        return true;
    }

    inline bool reportThen(const char* then)
    {
        DbgPrint("    THEN: %s\n", then);
        return true;
    }

    struct Clause
    {
        int given;
        int when;
        int then;

        bool operator==(const Clause& other) const
        {
            return given == other.given && when == other.when && then == other.then;
        }
    };

    typedef void(*TestFunc)(Clause curClause, Clause& nextClause, int& assertions);

    class TestEntry
    {
    public:
        void run(int& assertions) const
        {
            Clause curClause = {};
            Clause nextClause = {};
            int localAssertions = 0;

            for (;;)
            {
                m_func(curClause, nextClause, localAssertions);

                if (nextClause == curClause)
                {
                    break;
                }

                curClause = nextClause;
            }

            reportScenarioEnd(localAssertions);
            assertions += localAssertions;
        }

    private:
        TestEntry();

    private:
        const TestFunc m_func;
    };

    __declspec(selectany) __declspec(allocate("KMTEST$__a")) const TestEntry* testEntryA = nullptr;
    __declspec(selectany) __declspec(allocate("KMTEST$__z")) const TestEntry* testEntryZ = nullptr;

    inline void run()
    {
        DbgPrint("**************************************************\n");
        DbgPrint("* KMTEST BEGIN\n");
        DbgPrint("**************************************************\n");

        int scenarios = 0;
        int assertions = 0;

        for (const TestEntry** testEntry = &testEntryA; testEntry < &testEntryZ; ++testEntry)
        {
            if (!*testEntry)
            {
                continue;
            }

            (*testEntry)->run(assertions);
            ++scenarios;
        }

        DbgPrint("**************************************************\n");
        DbgPrint("* KMTEST END (scenarios: %d, assertions: %d)\n", scenarios, assertions);
        DbgPrint("**************************************************\n");
    }
}

DRIVER_UNLOAD DriverUnload;

inline void DriverUnload(_In_ DRIVER_OBJECT*)
{
}

extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" inline NTSTATUS DriverEntry(_In_ DRIVER_OBJECT* driverObject, _In_ PUNICODE_STRING)
{
    kmtest::run();

    driverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}