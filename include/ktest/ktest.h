#pragma once

#define KTEST_CAT2(x, y)        x##y
#define KTEST_CAT(x, y)         KTEST_CAT2(x, y)

#define KTEST_MAKE_ID(prefix)   KTEST_CAT(prefix, __LINE__)

#define SCENARIO(name) \
    namespace ktest \
    { \
        static void KTEST_MAKE_ID(testFunc)(Clause curClause, Clause& nextClause, int& assertions, bool nextClauseSet = false); \
        static void KTEST_MAKE_ID(testFuncStub)(Clause curClause, Clause& nextClause, int& assertions) \
        { \
            if (curClause == Clause()) reportScenarioBegin(name); \
            KTEST_MAKE_ID(testFunc)(curClause, nextClause, assertions); \
        } \
        namespace \
        { \
            const TestFunc KTEST_MAKE_ID(testEntry) = KTEST_MAKE_ID(testFuncStub);  \
            __declspec(dllexport) __declspec(allocate("KTEST$__m")) auto KTEST_MAKE_ID(testEntryPtr) = reinterpret_cast<const TestEntry*>(&KTEST_MAKE_ID(testEntry)); \
        } \
    } \
    __pragma(warning(suppress: 4100 /*unreferenced formal parameter*/)) \
    static void KTEST_MAKE_ID(ktest::testFunc)(Clause curClause, Clause& nextClause, int& assertions, bool nextClauseSet)

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

#pragma section("KTEST$__a", read)
#pragma section("KTEST$__m", read)
#pragma section("KTEST$__z", read)
#pragma comment(linker, "/merge:KTEST=.rdata")

namespace ktest
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

    __declspec(selectany) __declspec(allocate("KTEST$__a")) const TestEntry* testEntryA = nullptr;
    __declspec(selectany) __declspec(allocate("KTEST$__z")) const TestEntry* testEntryZ = nullptr;

    inline void run()
    {
        DbgPrint("**************************************************\n");
        DbgPrint("* KTEST BEGIN\n");
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
        DbgPrint("* KTEST END (scenarios: %d, assertions: %d)\n", scenarios, assertions);
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
    ktest::run();

    driverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}