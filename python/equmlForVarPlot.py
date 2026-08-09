import numpy as np
import torch
import torch.nn as nn
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

MODEL_FILE = "equ_surrogate_model_best.pt"
STATS_FILE = "equ_norm_stats.npz"
OUT_IMAGE = "equ_force_direction_effect.png"

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

def preprocess_X(X):
    """
    Convert raw X = [L, W, H, E, nu, Fx, Fy, Fz]
    into a reduced representation using symmetry:
      - canonicalize W/H so W >= H
      - convert Fy/Fz into force magnitude + angle
    Returns:
      [L, W, H, E, nu, Fx, force_mag, force_angle]
    """
    X = np.asarray(X, dtype=np.float64)
    X_new = []

    for row in X:
        L, W, H, E, nu, Fx, Fy, Fz = row

        # Use symmetry in the y-z plane
        Fy = abs(Fy)
        Fz = abs(Fz)

        # Canonical ordering for cuboid sides
        # If W < H, swap them and swap the force components too
        # because y/z directions are interchangeable.
        if W < H:
            W, H = H, W
            Fy, Fz = Fz, Fy

        force_mag = np.sqrt(Fy**2 + Fz**2)
        force_angle = np.arctan2(Fz, Fy)   # angle in [0, pi/2] after abs()

        X_new.append([L, W, H, E, nu, Fx, force_mag, force_angle])

    return np.array(X_new, dtype=np.float64)

def predict_case(L, W, H, E, nu, Fx, Fy, Fz):
    x_raw = np.array([[L, W, H, E, nu, Fx, Fy, Fz]], dtype=np.float64)
    x = preprocess_X(x_raw)
    x_norm = (x - X_mean) / X_std

    with torch.no_grad():
        y_norm = model(torch.tensor(x_norm, dtype=torch.float32)).cpu().numpy()

    y = y_norm * Y_std + Y_mean
    return y[0]

def sweep_force_direction(L):
    #L, W, H = 3.0, 3.0, 3.0
    W, H = 3.0, 3.0
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

def plot_results(angles, preds,filename):
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
        f"(force rotated in x-y plane, magnitude fixed at 1000), filename = {filename} ",
        fontsize=14
    )
    fig.tight_layout(rect=[0, 0.03, 1, 0.95])
    fig.savefig(filename, dpi=200)
    print(f"Saved figure to {filename}")

if __name__ == "__main__":
    for i in range(1,5):     
        outIm = f"xyForcesweepL{i}.png"
        angles, preds = sweep_force_direction(i)
        plot_results(angles, preds, outIm)
