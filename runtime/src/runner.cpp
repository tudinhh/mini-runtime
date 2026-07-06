#include <iostream>
#include <dlfcn.h>
#include <cstdint>      
#include "MnrtDevice.h"
#include "CpuDevice.h"

template <typename T, size_t N>
struct MemRefDescriptor {
  T *allocatedPtr;
  T *alignedPtr;
  intptr_t offset;
  intptr_t sizes[N];
  intptr_t strides[N];
};

int main() {
    // 1. Initialize the target device
    MnrtDevice* device = new CpuDevice();

    // 2. Load the compiled binary
    void* handle = dlopen("/home/anhtu/cnn-mlir-tutorial/model/build/model.so", RTLD_LAZY);
    void* kernel_sym = dlsym(handle, "_mlir_ciface_forward");

    // 3. Allocate device memory via the HAL
    size_t bytes = 4 * sizeof(float);
    float* in_ptr = static_cast<float*>(device->allocate(bytes));
    float* out_ptr = static_cast<float*>(device->allocate(bytes));

    for(int i = 0; i < 4; i++) in_ptr[i] = 1.0f; // Initialize input

    // 4. Construct the C-ABI descriptors
    MemRefDescriptor<float, 2> in_desc = {in_ptr, in_ptr, 0, {2, 2}, {2, 1}};
    MemRefDescriptor<float, 2> out_desc = {out_ptr, out_ptr, 0, {2, 2}, {2, 1}};

    // 5. Pack arguments and submit to device queue
    void* args[] = {&out_desc, &in_desc};
    device->submit(kernel_sym, args);

    // 6. Verify and Cleanup
    std::cout << "Output[0]: " << out_ptr[0] << "\n";
    
    device->deallocate(in_ptr);
    device->deallocate(out_ptr);
    delete device;
    dlclose(handle);

    return 0;
}