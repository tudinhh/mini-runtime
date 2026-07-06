#pragma once
#include <cstddef>

class MnrtDevice {
    public:
    virtual ~MnrtDevice() = default;
    virtual void* allocate(size_t size, size_t alignment = 64) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual void submit(void* kernel_ptr, void* args) = 0;
};