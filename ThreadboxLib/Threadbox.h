#pragma once

#include "Thread.h"

#include <unordered_map>
#include <memory>
#include <string>

class Threadbox
{
public:
    Threadbox();
    ~Threadbox();

    Threadbox(const Threadbox&) = delete;
    Threadbox& operator=(const Threadbox&) = delete;

    ThreadId CreateThread(std::function<ThreadFunc> threadfunc);
    void* SwitchTo(ThreadId id, void* param = nullptr);
    void SetThreadParent(ThreadId id, ThreadId parent_id);

    ThreadId current_id() const { return current_id_; }

private:
    ThreadId current_id_ = MainThreadId;
    ThreadId next_id_ = MainThreadId + 1;
    std::unordered_map<ThreadId, std::unique_ptr<Thread>> threads_;
    void* main_sp_ = nullptr;
    ThreadId caller_id_ = MainThreadId; // The last thread that made the switch
};
