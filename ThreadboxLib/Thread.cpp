#include "Thread.h"
#include "Threadbox.h"
#include "switch_stack.h"
#include <errno.h>

// macOS specific
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <exception>

#define STACK_NUM_PAGES 1

static const int page_size = getpagesize();

void Thread::ThreadMain(Thread* thread) {
    Thread* t = thread; // We have to use the local copy from here on
    // This pointer on the stack is the reason why Thread objects cannot be moved

    // Go back to the constructor
    void* param = switch_stack(&t->sp_, t->sp_in_ctor_, nullptr);

    // Execute the main function of the thread
    void* result = t->threadfunc_(param);

    // Switch to the parent thread
    t->finished_ = true;
    t->tb_->SwitchTo(t->parent_id_, result);
}

Thread::Thread(Threadbox* tb, std::function<ThreadFunc> threadfunc, ThreadId parent_id)
: tb_(tb), threadfunc_(threadfunc), parent_id_(parent_id) {
    AllocateStack();

    auto* p = static_cast<uint64_t*>(stack_);
    const int last_index = page_size / sizeof(uint64_t);
    p[last_index - 2] = reinterpret_cast<uint64_t>(ThreadMain); // RIP on stack
    p[last_index - 3] = 0; // RBP on stack

    sp_ = &p[last_index - 3]; // RSP

    // Switch to thread_main to save `this` in a local variable
    switch_stack(&sp_in_ctor_, sp_, this);
}

Thread::~Thread() {
    FreeStack();
}

void Thread::AllocateStack() {
    no_write_region_ = mmap(0, (1 + STACK_NUM_PAGES)*page_size, PROT_NONE,
        MAP_ANON | MAP_PRIVATE, -1, 0);
    if (no_write_region_ == MAP_FAILED) {
        fprintf(stderr, "mmap() failed: %s\n", strerror(errno));
        return;
    }

    stack_ = mmap(static_cast<char*>(no_write_region_) + page_size, STACK_NUM_PAGES*page_size, PROT_WRITE,
        MAP_FIXED | MAP_ANON | MAP_PRIVATE, -1, 0);
    if (stack_ == MAP_FAILED) {
        fprintf(stderr, "mmap() failed: %s\n", strerror(errno));
        return;
    }

    // Make sure the stack is 16-byte aligned
    assert((reinterpret_cast<uint64_t>(stack_) & 0x0f) == 0);
}

void Thread::FreeStack() {
    if (stack_) {
        if (munmap(stack_, STACK_NUM_PAGES*page_size) != 0) {
            fprintf(stderr, "munmap() failed: %s\n", strerror(errno));
        }
        stack_ = nullptr;
    }

    if (no_write_region_)
    {
        if (munmap(no_write_region_, (1 + STACK_NUM_PAGES)*page_size) != 0) {
            fprintf(stderr, "munmap() failed: %s\n", strerror(errno));
        }
        no_write_region_ = nullptr;
    }
}
