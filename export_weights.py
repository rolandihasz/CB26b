"""
Export PyTorch 'eagle_eye_model.pth' to flat binary 'eagle_eye_weights.bin'
for high-speed pure C++ inference in EagleEyeClassifier.
"""

import os
import sys
import torch

def export_weights(model_path="various/EagleEye/eagle_eye_model.pth", output_path="eagle_eye_weights.bin"):
    # Fallback to local file if path not found
    if not os.path.exists(model_path):
        if os.path.exists("eagle_eye_model.pth"):
            model_path = "eagle_eye_model.pth"
        elif os.path.exists("../various/EagleEye/eagle_eye_model.pth"):
            model_path = "../various/EagleEye/eagle_eye_model.pth"

    print(f"Loading PyTorch weights from: {model_path}")
    sd = torch.load(model_path, map_location="cpu", weights_only=True)

    # Expected layer keys in exact order for EagleEyeClassifier:
    keys = [
        "conv1.weight",  # [16, 1, 3, 3]  = 144 floats
        "conv1.bias",    # [16]           = 16 floats
        "conv2.weight",  # [32, 16, 3, 3] = 4608 floats
        "conv2.bias",    # [32]           = 32 floats
        "fc1.weight",    # [128, 1568]    = 200704 floats
        "fc1.bias",      # [128]          = 128 floats
        "fc2.weight",    # [10, 128]      = 1280 floats
        "fc2.bias"       # [10]           = 10 floats
    ]

    total_floats = 0
    with open(output_path, "wb") as f:
        for key in keys:
            if key not in sd:
                raise KeyError(f"Missing key in state_dict: {key}")
            tensor = sd[key].float().cpu().numpy()
            total_floats += tensor.size
            f.write(tensor.tobytes())
            print(f"  Exported {key:14s}: shape {str(tensor.shape):18s} ({tensor.size} floats)")

    file_size = os.path.getsize(output_path)
    print(f"\nSuccessfully wrote {output_path} ({file_size / 1024:.2f} KB)")

    # Also copy to cb26 root if running from inside various/EagleEye
    if os.path.exists("../../cb26.pro"):
        import shutil
        shutil.copyfile(output_path, "../../eagle_eye_weights.bin")
        print("Also updated ../../eagle_eye_weights.bin in cb26 root!")

if __name__ == "__main__":
    src = sys.argv[1] if len(sys.argv) > 1 else "various/EagleEye/eagle_eye_model.pth"
    dst = sys.argv[2] if len(sys.argv) > 2 else "eagle_eye_weights.bin"
    export_weights(src, dst)
