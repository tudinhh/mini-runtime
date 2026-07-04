#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <cstdint>

// 1. Define the MLIR MemRef Descriptor
template <typename T, size_t Rank>
struct MemRefDescriptor {
    T *allocatedPtr;
    T *alignedPtr;
    intptr_t offset;
    intptr_t sizes[Rank];
    intptr_t strides[Rank];
};

// Assuming you used float32. If you kept i32 in MLIR, change 'float' to 'int32_t'
typedef MemRefDescriptor<float, 2> MemRef2D;

extern "C" {
    // 2. The C-ABI signature: Return value becomes the FIRST pointer argument.
    // If your Python exporter used 'main', change this to _mlir_ciface_main
    void _mlir_ciface_forward(MemRef2D* out, MemRef2D* arg0, MemRef2D* arg1);
}

// Function pointer type matching the signature
typedef void (*ModelFunc)(MemRef2D*, MemRef2D*, MemRef2D*);

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    void* handle = dlopen(argv[1], RTLD_LAZY);
    if (!handle) {
        std::cerr << "Load error: " << dlerror() << '\n';
        return 1;
    }

    // Load the symbol
    ModelFunc forward_pass = (ModelFunc)dlsym(handle, "_mlir_ciface_forward");
    if (!forward_pass) {
        std::cerr << "Symbol error: " << dlerror() << '\n';
        return 1;
    }

    // 3. Allocate raw memory
    std::vector<float> data_x(4, 1.0f);
    std::vector<float> data_y(4, 2.0f);
    
    // We don't allocate data_out here because one-shot-bufferize usually 
    // allocates the output buffer internally and returns it.

    // 4. Setup the MemRef Structs for a 2x2 tensor
    MemRef2D memref_x = {
        data_x.data(), data_x.data(), 0, {2, 2}, {2, 1}
    };
    
    MemRef2D memref_y = {
        data_y.data(), data_y.data(), 0, {2, 2}, {2, 1}
    };

    MemRef2D memref_out; // The compiled function will populate this

    // 5. Dispatch
    std::cout << "[Runtime] Dispatching to compiled kernel...\n";
    forward_pass(&memref_out, &memref_x, &memref_y);

    // 6. Verify Results
    std::cout << "[Runtime] Output:\n";
    float* out_data = memref_out.alignedPtr;
    for(int i = 0; i < 4; ++i) {
        std::cout << out_data[i] << " ";
    }
    std::cout << "\n";

    // Optional: Free the memory allocated by the MLIR kernel
    // free(memref_out.allocatedPtr);

    dlclose(handle);
    return 0;
}