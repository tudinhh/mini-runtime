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
        auto kernel = reinterpret_cast<void (*)(void*)>(kernel_ptr);
        kernel(args);
    }
};