#include "Threadbox.h"
#include <gtest/gtest.h>

TEST(ThreadboxLib, SwitchToCallsThreadFunc) {
    Threadbox tb;

    bool has_run = false;
    ThreadId id = tb.CreateThread([&has_run](void*) {
        has_run = true;
        return nullptr;
    });
    EXPECT_EQ(has_run, false);
    void* retval = tb.SwitchTo(id, reinterpret_cast<void*>(42));
    EXPECT_EQ(has_run, true);
    EXPECT_EQ(retval, nullptr);
}

TEST(ThreadboxLib, SwitchToParamsAndReturnValue) {
    Threadbox tb;

    ThreadId id = tb.CreateThread([](void* p) {
        return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(p) + 1);
    });
    void* retval = tb.SwitchTo(id, reinterpret_cast<void*>(42));
    EXPECT_EQ(retval, reinterpret_cast<void*>(43));
}

TEST(ThreadboxLib, ThreadFuncReturnsToParent) {
    Threadbox tb;

    std::string s;
    ThreadId id1 = tb.CreateThread([&tb, &s](void*) {
        s += '2';
        ThreadId id2 = tb.CreateThread([&s](void*) {
            s += '4';
            return nullptr; // returns to Thread_1
        });
        s += '3';
        tb.SwitchTo(id2, nullptr);
        s += '5';
        return nullptr; // returns to MainThread
    });
    s += '1';
    tb.SwitchTo(id1, nullptr);
    s += '6';

    EXPECT_STREQ(s.c_str(), "123456");
}

TEST(ThreadboxLib, ThreadFuncReturnsToParent2) {
    Threadbox tb;

    std::string s;
    ThreadId id2 = 0;
    ThreadId id1 = tb.CreateThread([&tb, &id2, &s](void*) {
        s += '2';
        tb.SwitchTo(id2);
        s += '4'; // unreachable code
        return nullptr;
    });
    id2 = tb.CreateThread([&tb, &s](void*) { // Thread_2 is created by MainThread
        s += '3';
        return nullptr;
    });
    s += '1';
    tb.SwitchTo(id1, nullptr);
    s += '5';

    EXPECT_STREQ(s.c_str(), "1235");
}

TEST(ThreadboxLib, ChangeParentThread) {
    Threadbox tb;

    std::string s;
    ThreadId id1 = tb.CreateThread([&tb, &s](void*) {
        s += '2';
        return nullptr;
    });
    ThreadId id2 = tb.CreateThread([&tb, &s](void*) {
        s += '3';
        return nullptr;
    });
    s += '1';
    tb.SetThreadParent(id1, id2);
    tb.SwitchTo(id1, nullptr);
    s += '4';

    EXPECT_STREQ(s.c_str(), "1234");
}


// Switching to the same thread is an error
// Switching to a finished thread is an error
// Switching to a thread that doesn't exist is an error (this covers the case when the target thread belongs to another ThreadBox)
// Destroying threadbox should crash with a message if there are unfinished threads (otherwise threads can leak resources)

// We could support exceptions:
// Exceptions return to the parent as well
// Exceptions can be triggered inside a thread: ThrowIn()
