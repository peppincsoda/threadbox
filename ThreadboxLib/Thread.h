#pragma once

#include <cstdint>
#include <functional>

using ThreadId = std::uint32_t;
const ThreadId MainThreadId = 1;

using ThreadFunc = void *(void*);

class Threadbox;

class Thread
{
    friend Threadbox;

public:
    Thread(Threadbox* tb, std::function<ThreadFunc> threadfunc, ThreadId parent_id);
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&&) = delete;
    Thread& operator=(Thread&&) = delete;

    bool finished() const { return finished_; }

    ThreadId parent_id() const { return parent_id_; }
    void set_parent_id(ThreadId parent_id) { parent_id_ = parent_id; }

private:
    void AllocateStack();
    void FreeStack();

    static void ThreadMain(Thread* thread);

    Threadbox* tb_ = nullptr;
    std::function<ThreadFunc> threadfunc_; // The function the thread will execute
    ThreadId parent_id_ = 0; // The thread to switch to when this thread finishes
    void* no_write_region_ = nullptr;
    void* stack_ = nullptr;
    void* sp_ = nullptr;

    void* sp_in_ctor_ = nullptr;
    bool finished_ = false;
};
