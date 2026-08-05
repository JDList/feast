import numpy as np
import torch
import torch.nn as nn

MODEL_FILE = "surrogate_model_best.pt"
STATS_FILE = "norm_stats.npz"

# ----------------------------
# Model definition must match training
# ----------------------------
class SurrogateNet(nn.Module):
    def __init__(self, input_dim, output_dim):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, 128),
            nn.ReLU(),
            nn.Linear(128, 128),
            nn.ReLU(),
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.Linear(64, output_dim),
        )

    def forward(self, x):
        return self.net(x)

# ----------------------------
# Load model and stats
# ----------------------------
ckpt = torch.load(MODEL_FILE, map_location="cpu")
stats = np.load(STATS_FILE)

X_mean = stats["X_mean"]
X_std = stats["X_std"]
Y_mean = stats["Y_mean"]
Y_std = stats["Y_std"]

model = SurrogateNet(ckpt["input_dim"], ckpt["output_dim"])
model.load_state_dict(ckpt["model_state_dict"])
model.eval()

def predict_case(L, W, H, E, nu, Fx, Fy, Fz):
    """
    Input must match the training feature order exactly.
    """
    x = np.array([[L, W, H, E, nu, Fx, Fy, Fz]], dtype=np.float64)
    x_norm = (x - X_mean) / X_std

    with torch.no_grad():
        y_norm = model(torch.tensor(x_norm, dtype=torch.float32)).numpy()

    y = y_norm * Y_std + Y_mean
    return y[0]

# Example
if __name__ == "__main__":
    pred = predict_case(
        L=1.0, W=1.0, H=1.0,
        E=210e7, nu=0.30,
        Fx=0.0, Fy=-1000.0, Fz=0.0
    )
    print("Predicted outputs:")
    print("Displacement magnitude")
    print("  Max               =", pred[0])
    print("  Mean              =", pred[1])
    print("  Standard deviation=", pred[2])

    print("\nVon Mises stress")
    print("  Max               =", pred[3])
    print("  Mean              =", pred[4])
    print("  Standard deviation=", pred[5])

    print("\nHydrostatic stress")
    print("  Mean              =", pred[6])

    print("\nPrincipal stresses")
    print("  Maximum principal =", pred[7])
    print("  Minimum principal =", pred[8])

    print("\nStrain")
    print("  Mean magnitude    =", pred[9])
