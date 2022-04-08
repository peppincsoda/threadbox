#include "Threadbox.h"
#include "switch_stack.h"

Threadbox::Threadbox() {
}

Threadbox::~Threadbox() {
}

ThreadId Threadbox::CreateThread(std::function<ThreadFunc> threadfunc) {
    const ThreadId id = next_id_++;
    threads_.emplace(id, std::make_unique<Thread>(this, threadfunc, current_id_));
    return id;
}

void* Threadbox::SwitchTo(ThreadId id, void *param) {

    assert(current_id_ != id && "Cannot switch to the same thread");

    void** p_current_sp = nullptr;
    if (current_id_ == MainThreadId) {
        p_current_sp = &main_sp_;
    } else {
        auto it = threads_.find(current_id_);
        assert(it != threads_.end() && "Current thread is invalid");
        p_current_sp = &it->second->sp_;
    }

    void* next_sp = nullptr;
    if (id == MainThreadId) {
        next_sp = main_sp_;
    } else {
        auto it = threads_.find(id);
        assert(it != threads_.end() && "Target thread does not exist");
        next_sp = it->second->sp_;
    }

    caller_id_ = current_id_;
    current_id_ = id;
    void* r = switch_stack(p_current_sp, next_sp, param);

    // Free the caller Thread after the switch if it's finished
    if (caller_id_ != MainThreadId) {
        auto& pThread = threads_.at(caller_id_);
        if (pThread->finished()) {
            threads_.erase(caller_id_);
        }
    }

    return r;
}

void Threadbox::SetThreadParent(ThreadId id, ThreadId parent_id) {
    auto it = threads_.find(id);
    assert(it != threads_.end() && "Thread does not exist");
    it->second->set_parent_id(parent_id);
}
