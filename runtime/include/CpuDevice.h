#pragma once
#include "MnrtDevice.h"
#include <cstdlib>

class CpuDevice : public MnrtDevice {
public:
    void* allocate(size_t size, size_t alignment = 64) override {
        void* ptr = nullptr;
        posix_memalign(&ptr, alignment, size);
        return ptr;
    }

    void deallocate(void* ptr) override {
        free(ptr);
    }

    void submit(void* kernel_ptr, void* args) override {
        // 1. Cast the kernel to expect two pointer arguments
        auto kernel = reinterpret_cast<void (*)(void*, void*)>(kernel_ptr);
        
        // 2. Cast the incoming args back to an array of pointers
        void** packed_args = static_cast<void**>(args);
        
        // 3. Dispatch with explicitly unpacked arguments
        kernel(packed_args[0], packed_args[1]);
    }
};