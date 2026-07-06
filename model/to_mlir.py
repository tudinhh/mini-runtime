import torch
from torch_mlir import torchscript

class AddModel(torch.nn.Module):
    def forward(self, a, b):
        return a + b

model = AddModel()
a = torch.ones(2, 2, dtype=torch.float)
b = torch.ones(2, 2, dtype=torch.float)

module_linalg = torchscript.compile(
    model, 
    (a, b), 
    output_type="linalg-on-tensors"
)
with open("./build/input.mlir", "w") as f:
    f.write(str(module_linalg))

print("Done")