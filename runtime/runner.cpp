#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <cstdint>
#include "CommandBuffer.h"

template <typename T, size_t Rank>
struct MemRefDescriptor {
    T *allocatedPtr;
    T *alignedPtr;
    intptr_t offset;
    intptr_t sizes[Rank];
    intptr_t strides[Rank];
};

typedef MemRefDescriptor<float, 2> MemRef2D;

extern "C" {
    void _mlir_ciface_forward(MemRef2D* out, MemRef2D* arg0, MemRef2D* arg1);
}

typedef void (*ModelFunc)(MemRef2D*, MemRef2D*, MemRef2D*);

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    void* handle = dlopen(argv[1], RTLD_LAZY);
    if (!handle) return 1;

    ModelFunc forward_pass = (ModelFunc)dlsym(handle, "_mlir_ciface_forward");
    if (!forward_pass) return 1;

    std::vector<float> data_x(4, 1.0f);
    std::vector<float> data_y(4, 2.0f);
    
    MemRef2D memref_x = {data_x.data(), data_x.data(), 0, {2, 2}, {2, 1}};
    MemRef2D memref_y = {data_y.data(), data_y.data(), 0, {2, 2}, {2, 1}};
    MemRef2D memref_out;

    CommandBuffer cmd_buf;

    cmd_buf.record([&]() {
        forward_pass(&memref_out, &memref_x, &memref_y);
    });

    cmd_buf.submit();

    float* out_data = memref_out.alignedPtr;
    for(int i = 0; i < 4; ++i) {
        std::cout << out_data[i] << " ";
    }
    std::cout << "\n";

    dlclose(handle);
    return 0;
}