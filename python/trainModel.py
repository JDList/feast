import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader

DATA_FILE = "fea_dataset.npz"
MODEL_FILE = "surrogate_model_best.pt"
STATS_FILE = "norm_stats.npz"

SEED = 42
BATCH_SIZE = 64
MAX_EPOCHS = 500
LR = 1e-3

# Early stopping settings
START_EARLY_STOPPING_AFTER = 100
PATIENCE = 100
MIN_DELTA = 0.0

torch.manual_seed(SEED)
np.random.seed(SEED)


class FEADataset(Dataset):
    def __init__(self, X, Y):
        self.X = torch.tensor(X, dtype=torch.float32)
        self.Y = torch.tensor(Y, dtype=torch.float32)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.Y[idx]


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


def split_data(X, Y, train_frac=0.7, val_frac=0.15):
    n = len(X)
    idx = np.random.permutation(n)

    n_train = int(train_frac * n)
    n_val = int(val_frac * n)

    train_idx = idx[:n_train]
    val_idx = idx[n_train:n_train + n_val]
    test_idx = idx[n_train + n_val:]

    return (
        X[train_idx], Y[train_idx],
        X[val_idx], Y[val_idx],
        X[test_idx], Y[test_idx],
    )


def evaluate_loss(model, loader, criterion):
    model.eval()
    total_loss = 0.0
    n_items = 0

    with torch.no_grad():
        for xb, yb in loader:
            pred = model(xb)
            loss = criterion(pred, yb)
            total_loss += loss.item() * xb.size(0)
            n_items += xb.size(0)

    return total_loss / max(n_items, 1)


def predict_numpy(model, X_np):
    model.eval()
    with torch.no_grad():
        X_t = torch.tensor(X_np, dtype=torch.float32)
        Y_t = model(X_t)
    return Y_t.cpu().numpy()


def main():
    data = np.load(DATA_FILE, allow_pickle=True)
    X = data["X"].astype(np.float64)
    Y = data["Y"].astype(np.float64)

    if "target_names" in data.files:
        target_names = data["target_names"]
    else:
        target_names = np.array([f"y{i}" for i in range(Y.shape[1])], dtype=object)

    X_train, Y_train, X_val, Y_val, X_test, Y_test = split_data(X, Y)

    # Normalize using training statistics only
    X_mean = X_train.mean(axis=0)
    X_std = X_train.std(axis=0) + 1e-8
    Y_mean = Y_train.mean(axis=0)
    Y_std = Y_train.std(axis=0) + 1e-8

    X_train_n = (X_train - X_mean) / X_std
    X_val_n = (X_val - X_mean) / X_std
    X_test_n = (X_test - X_mean) / X_std

    Y_train_n = (Y_train - Y_mean) / Y_std
    Y_val_n = (Y_val - Y_mean) / Y_std
    Y_test_n = (Y_test - Y_mean) / Y_std

    np.savez(
        STATS_FILE,
        X_mean=X_mean,
        X_std=X_std,
        Y_mean=Y_mean,
        Y_std=Y_std,
        target_names=target_names,
    )

    train_ds = FEADataset(X_train_n, Y_train_n)
    val_ds = FEADataset(X_val_n, Y_val_n)
    test_ds = FEADataset(X_test_n, Y_test_n)

    train_loader = DataLoader(train_ds, batch_size=BATCH_SIZE, shuffle=True)
    val_loader = DataLoader(val_ds, batch_size=BATCH_SIZE, shuffle=False)
    test_loader = DataLoader(test_ds, batch_size=BATCH_SIZE, shuffle=False)

    input_dim = X.shape[1]
    output_dim = Y.shape[1]

    model = SurrogateNet(input_dim, output_dim)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LR)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimizer, mode="min", factor=0.5, patience=10
    )

    best_val = float("inf")
    best_epoch = -1
    patience_left = PATIENCE
    best_state = None

    for epoch in range(1, MAX_EPOCHS + 1):
        model.train()
        total_train_loss = 0.0
        n_train = 0

        for xb, yb in train_loader:
            pred = model(xb)
            loss = criterion(pred, yb)

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            total_train_loss += loss.item() * xb.size(0)
            n_train += xb.size(0)

        train_loss = total_train_loss / max(n_train, 1)
        val_loss = evaluate_loss(model, val_loader, criterion)
        scheduler.step(val_loss)

        current_lr = optimizer.param_groups[0]["lr"]
        print(
            f"Epoch {epoch:03d} | "
            f"train {train_loss:.6f} | "
            f"val {val_loss:.6f} | "
            f"lr {current_lr:.6g}"
        )

        if val_loss < best_val - MIN_DELTA:
            best_val = val_loss
            best_epoch = epoch
            patience_left = PATIENCE

            best_state = {
                "model_state_dict": model.state_dict(),
                "input_dim": int(input_dim),
                "output_dim": int(output_dim),
                "best_epoch": int(best_epoch),
                "best_val_loss": float(best_val),
            }

            torch.save(best_state, MODEL_FILE)
        else:
            if epoch >= START_EARLY_STOPPING_AFTER:
                patience_left -= 1
                if patience_left <= 0:
                    print(f"Early stopping at epoch {epoch}, best epoch was {best_epoch}")
                    break

    if best_state is None:
        raise RuntimeError("Training failed to produce a valid checkpoint.")

    # Restore best model from memory
    model.load_state_dict(best_state["model_state_dict"])
    model.eval()

    val_loss_best = evaluate_loss(model, val_loader, criterion)
    test_loss = evaluate_loss(model, test_loader, criterion)

    print(f"\nBest validation loss: {best_val:.6f} at epoch {best_epoch}")
    print(f"Validation loss now:  {val_loss_best:.6f}")
    print(f"Final test loss:      {test_loss:.6f}")

    # Per-output error in original units
    Y_pred_test_n = predict_numpy(model, X_test_n)
    Y_pred_test = Y_pred_test_n * Y_std + Y_mean

    abs_err = np.abs(Y_pred_test - Y_test)
    rmse = np.sqrt(np.mean((Y_pred_test - Y_test) ** 2, axis=0))
    mae = np.mean(abs_err, axis=0)

    print("\nPer-output test error in original units:")
    for i, name in enumerate(target_names):
        print(f"{name}: MAE={mae[i]:.6g}, RMSE={rmse[i]:.6g}")

    print(f"\nSaved best model to {MODEL_FILE}")
    print(f"Saved normalization stats to {STATS_FILE}")


if __name__ == "__main__":
    main()
