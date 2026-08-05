import numpy as np
import torch
import torch.nn as nn
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

MODEL_FILE = "surrogate_model_best.pt"
STATS_FILE = "norm_stats.npz"
OUT_IMAGE = "force_direction_effect.png"

# Must match training
TARGET_NAMES = [
    "displacement magnitude max",
    "displacement magnitude mean",
    "displacement magnitude std",
    "von Mises stress max",
    "von Mises stress mean",
    "von Mises stress std",
    "hydrostatic stress mean",
    "maximum principal stress max",
    "minimum principal stress min",
    "strain magnitude mean",
]

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

ckpt = torch.load(MODEL_FILE, map_location="cpu")
stats = np.load(STATS_FILE, allow_pickle=True)

X_mean = stats["X_mean"]
X_std = stats["X_std"]
Y_mean = stats["Y_mean"]
Y_std = stats["Y_std"]

model = SurrogateNet(ckpt["input_dim"], ckpt["output_dim"])
model.load_state_dict(ckpt["model_state_dict"])
model.eval()

def predict_case(L, W, H, E, nu, Fx, Fy, Fz):
    x = np.array([[L, W, H, E, nu, Fx, Fy, Fz]], dtype=np.float64)
    x_norm = (x - X_mean) / X_std

    with torch.no_grad():
        y_norm = model(torch.tensor(x_norm, dtype=torch.float32)).cpu().numpy()

    y = y_norm * Y_std + Y_mean
    return y[0]

def sweep_force_direction():
    L, W, H = 3.0, 3.0, 3.0
    E, nu = 210e7, 0.30
    magnitude = 1000.0

    angles = np.linspace(0.0, 2.0 * np.pi, 181)
    preds = []

    for theta in angles:
        Fz = magnitude * np.cos(theta)
        Fy = magnitude * np.sin(theta)
        Fx = 0.0
        preds.append(predict_case(L, W, H, E, nu, Fx, Fy, Fz))

    return angles, np.array(preds)

def plot_results(angles, preds):
    deg = np.degrees(angles)
    n_outputs = preds.shape[1]
    ncols = 2
    nrows = int(np.ceil(n_outputs / ncols))

    fig, axes = plt.subplots(nrows, ncols, figsize=(16, 4 * nrows), sharex=True)
    axes = np.array(axes).reshape(-1)

    for i in range(n_outputs):
        ax = axes[i]
        ax.plot(deg, preds[:, i])
        ax.set_title(TARGET_NAMES[i])
        ax.set_xlabel("Force direction angle (degrees)")
        ax.set_ylabel("Predicted value")
        ax.grid(True)

    for j in range(n_outputs, len(axes)):
        axes[j].axis("off")

    fig.suptitle(
        "Effect of force direction on predicted outputs\n"
        "(force rotated in x-y plane, magnitude fixed at 1000)",
        fontsize=14
    )
    fig.tight_layout(rect=[0, 0.03, 1, 0.95])
    fig.savefig(OUT_IMAGE, dpi=200)
    print(f"Saved figure to {OUT_IMAGE}")

if __name__ == "__main__":
    angles, preds = sweep_force_direction()
    plot_results(angles, preds)
