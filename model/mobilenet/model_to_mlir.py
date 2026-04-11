import torch
import struct
import torchvision.transforms as transforms
import torchvision
from torch_mlir.fx import export_and_import

# 1. Fetch Model
model = torch.hub.load("chenyaofo/pytorch-cifar-models", "cifar10_mobilenetv2_x1_0", pretrained=True)
model.eval()
model.requires_grad_(False)

# THE FIX: The Buffer Hack
# Forcefully convert all mutable "buffers" (BatchNorm stats) into static "Parameters".
# This tells FX to embed them as dense_resources instead of lifting them to arguments.
for module in model.modules():
    for name, buf in list(module.named_buffers(recurse=False)):
        delattr(module, name) # Remove the buffer
        module.register_parameter(name, torch.nn.Parameter(buf, requires_grad=False)) # Add it back as a static parameter

dummy_input = torch.randn(1, 3, 32, 32)

# 2. Export using FX Backend
with torch.no_grad():
    module = export_and_import(
        model, 
        dummy_input, 
        output_type="linalg-on-tensors",
        func_name="forward"
    )

with open("build/mobilenet.mlir", "w") as f:
    f.write(str(module))
print("Model exported to cifar10_mobilenet.mlir using FX!")

# 4. Fetch CIFAR-10 and Export 100 images to Binary
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], std=[0.2023, 0.1994, 0.2010])
])

cifar_test = torchvision.datasets.CIFAR10(root='./data', train=False, download=True, transform=transform)
test_loader = torch.utils.data.DataLoader(cifar_test, batch_size=1, shuffle=False)

exported = 0
with open("build/cifar10_test.bin", "wb") as f:
    for images, labels in test_loader:
        if exported >= 100:
            break
        f.write(struct.pack('i', labels[0].item()))
        f.write(images[0].numpy().tobytes())
        exported += 1

print(f"Exported {exported} CIFAR-10 images to cifar10_test.bin")