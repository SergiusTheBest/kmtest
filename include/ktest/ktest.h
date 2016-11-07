#pragma once

#define KTEST_CAT2(x, y)        x##y
#define KTEST_CAT(x, y)         KTEST_CAT2(x, y)

#define KTEST_MAKE_ID(prefix)   KTEST_CAT(prefix, __LINE__)

#define SCENARIO(name) \
    namespace ktest \
    { \
        static void KTEST_MAKE_ID(testFunc)(Clause curClause, Clause& nextClause, ClauseDesc& clauseDesc); \
        static void KTEST_MAKE_ID(testFuncStub)(Clause curClause, Clause& nextClause, ClauseDesc& clauseDesc) \
        { \
            clauseDesc.scenario = name; \
            KTEST_MAKE_ID(testFunc)(curClause, nextClause, clauseDesc); \
        } \
        namespace \
        { \
            const TestFunc KTEST_MAKE_ID(testEntry) = KTEST_MAKE_ID(testFuncStub);  \
            __declspec(dllexport) __declspec(allocate("KTEST$__m")) auto KTEST_MAKE_ID(testEntryPtr) = reinterpret_cast<const TestEntry*>(&KTEST_MAKE_ID(testEntry)); \
        } \
    } \
    static void KTEST_MAKE_ID(ktest::testFunc)(Clause curClause, Clause& nextClause, ClauseDesc& clauseDesc)

#define GIVEN(desc) \
    if (curClause.given == 0) curClause.given = __LINE__; \
    if (curClause.given < __LINE__) { nextClause.given = __LINE__; nextClause.when = nextClause.then = 0; return; } \
    else if ((clauseDesc.given = desc, curClause.given) == __LINE__)

#define WHEN(desc) \
    if (curClause.when == 0) curClause.when = __LINE__; \
    if (curClause.when < __LINE__) { nextClause.when = __LINE__; nextClause.then = 0; return; } \
    else if ((clauseDesc.when = desc, curClause.when) == __LINE__)

#define THEN(desc) \
    if (curClause.then == 0) curClause.then = __LINE__; \
    if (curClause.then < __LINE__) { nextClause.then = __LINE__; return; } \
    else if ((clauseDesc.then = desc, curClause.then) ==  __LINE__)

#define REQUIRE(expression) \
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

    struct ClauseDesc
    {
        const char* scenario;
        const char* given;
        const char* when;
        const char* then;
    };

    typedef void(*TestFunc)(Clause curClause, Clause& nextClause, ClauseDesc& clauseDesc);

    class TestEntry
    {
    public:
        void run() const
        {
            DbgPrint("--------------------------------------------------\n");

            ClauseDesc clauseDesc = {};
            Clause curClause = {};

            for (int i = 0;; ++i)
            {
                Clause nextClause = curClause;

                m_func(curClause, nextClause, clauseDesc);

                if (0 == i)
                {
                    DbgPrint("SCENARIO: %s\n", clauseDesc.scenario);
                    DbgPrint("--------------------------------------------------\n");
                }

                DbgPrint("GIVEN: %s\n", clauseDesc.given);
                DbgPrint("  WHEN: %s\n", clauseDesc.when);
                DbgPrint("    THEN: %s\n", clauseDesc.then);

                if (nextClause == curClause)
                {
                    break;
                }

                curClause = nextClause;
            }

            DbgPrint("\n");
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

        for (const TestEntry** testEntry = &testEntryA; testEntry < &testEntryZ; ++testEntry)
        {
            if (!*testEntry)
            {
                continue;
            }

            (*testEntry)->run();
            ++scenarios;
        }

        DbgPrint("**************************************************\n");
        DbgPrint("* KTEST END (scenarios: %d)\n", scenarios);
        DbgPrint("**************************************************\n");
    }
}

DRIVER_UNLOAD Unload;

VOID Unload(_In_ DRIVER_OBJECT*) 
{
}

extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" inline NTSTATUS DriverEntry(_In_ DRIVER_OBJECT* driverObject, _In_ PUNICODE_STRING)
{    
    ktest::run();

    driverObject->DriverUnload = Unload;
    return STATUS_SUCCESS;
}