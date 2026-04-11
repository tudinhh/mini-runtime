#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

template <typename T, int N>
struct MemRefDescriptor {
  T* allocated;
  T* aligned;
  intptr_t offset;
  intptr_t sizes[N];
  intptr_t strides[N];
};

extern "C" {
void forward(MemRefDescriptor<float, 2>* output,
             MemRefDescriptor<float, 4>* input);
}

int main() {
  const int batch_size = 1;
  const int out_features = 10;

  // Allocate on the Heap to prevent stack overflow and ensure alignment
  std::vector<float> inputDataVec(batch_size * 3 * 32 * 32, 0.0f);
  std::vector<float> outputDataVec(batch_size * out_features, 0.0f);

  float* inputData = inputDataVec.data();
  float* outputData = outputDataVec.data();

  MemRefDescriptor<float, 4> inputMemRef = {
      inputData, inputData, 0, {batch_size, 3, 32, 32}, {3072, 1024, 32, 1}};

  MemRefDescriptor<float, 2> outputMemRef = {
      outputData, outputData, 0, {batch_size, out_features}, {out_features, 1}};

  std::ifstream dataset("build/cifar10_test.bin", std::ios::binary);
  if (!dataset) {
    std::cerr << "Error: Could not open cifar10_test.bin\n";
    return 1;
  }

  int correct_predictions = 0;
  int total_images = 0;
  int true_label;

  std::cout << "Starting MobileNet CIFAR-10 Evaluation...\n";

  // Read sequentially: 1 label (int), then 3072 pixels (floats)
  while (dataset.read(reinterpret_cast<char*>(&true_label), sizeof(int))) {
    dataset.read(reinterpret_cast<char*>(inputData),
                 sizeof(float) * 3 * 32 * 32);

    // Run MLIR Inference
    std::cout << "Inference...\n";
    forward(&outputMemRef, &inputMemRef);
    
    // Find predicted class (Argmax)
    int predicted_label = 0;
    float max_val = outputData[0];
    for (int i = 1; i < out_features; i++) {
      if (outputData[i] > max_val) {
        max_val = outputData[i];
        predicted_label = i;
      }
    }
    std::cout << "Score...\n";
    // Score against true label
    if (predicted_label == true_label) {
      correct_predictions++;
    }
    total_images++;

    if (total_images % 10 == 0) {
      std::cout << "Processed " << total_images << " images...\n";
    }
    for(int i=0; i<10; i++) std::cout << outputData[i] << " ";
std::cout << "\n";
  }

  if (total_images > 0) {
    float accuracy = (float)correct_predictions / total_images * 100.0f;
    std::cout << "\n=== Evaluation Complete ===\n";
    std::cout << "Total Images Processed: " << total_images << "\n";
    std::cout << "Accuracy: " << accuracy << "%\n";
  } else {
    std::cout << "No images found in dataset.\n";
  }

  return 0;
}