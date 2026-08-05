import traceback
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, as_completed

import numpy as np
import feast
from feastFuncs import getCuboid, buildMesh, fixRegion, getPP

N_SAMPLES = 5000
OUT_FILE = "fea_dataset.npz"

# Start conservatively. If your solver is heavy, 4-8 is often better than "all cores".
NUM_WORKERS = 4

# Reproducible random sampling across processes
MASTER_SEED = 42


def sample_unit_vector(rng):
    """Random 3D unit vector."""
    v = rng.normal(size=3)
    v /= np.linalg.norm(v) + 1e-12
    return v


def sample_case(rng):
    """
    Randomly sample one cuboid + material + load case.
    """
    # Shape
    L = rng.uniform(1.0, 5.0)
    W = rng.uniform(1.0, 5.0)
    H = rng.uniform(1.0, 5.0)

    # Material
    E = rng.uniform(50e7, 300e7)
    nu = rng.uniform(0.20, 0.45)

    # Force on highx face only
    magnitude = rng.uniform(0.0, 5000.0)
    direction = sample_unit_vector(rng)
    Fx, Fy, Fz = magnitude * direction

    x = np.array([L, W, H, E, nu, Fx, Fy, Fz], dtype=np.float64)

    meta = {
        "L": L, "W": W, "H": H,
        "E": E, "nu": nu,
        "Fx": Fx, "Fy": Fy, "Fz": Fz,
        "force_mag": magnitude,
    }
    return x, meta


def safe_scalar(value):
    """Convert solver outputs to plain float."""
    try:
        return float(value)
    except Exception:
        return np.nan


def run_one_fea_case(meta):
    """
    Run your FEA workflow for one sampled case.
    Returns a 10-value target vector y.
    """
    L, W, H = meta["L"], meta["W"], meta["H"]
    E, nu = meta["E"], meta["nu"]
    Fx, Fy, Fz = meta["Fx"], meta["Fy"], meta["Fz"]

    cuboid = getCuboid(L, W, H)
    build = buildMesh(cuboid)
    mesh = build.mesh

    region_bcs = feast.RegionBoundaryConditionSet()
    region_bcs = fixRegion(region_bcs, "lowx")

    # Force applied only to highx face.
    region_bcs.addRegionNeumann("highx", 0, Fx)
    region_bcs.addRegionNeumann("highx", 1, Fy)
    region_bcs.addRegionNeumann("highx", 2, Fz)

    resolved_bcs = feast.BoundaryConditionResolver.resolve(region_bcs, build)
    material = feast.LinearElastic(E, nu)
    pp = getPP(mesh, resolved_bcs, material)

    y = np.array([
        safe_scalar(pp.displacement.magnitude.summary.max),
        safe_scalar(pp.displacement.magnitude.summary.mean),
        safe_scalar(pp.displacement.magnitude.summary.standardDeviation),

        safe_scalar(pp.stress.vonMises.summary.max),
        safe_scalar(pp.stress.vonMises.summary.mean),
        safe_scalar(pp.stress.vonMises.summary.standardDeviation),

        safe_scalar(pp.stress.hydrostatic.summary.mean),

        safe_scalar(pp.stress.principal.maximum.summary.max),
        safe_scalar(pp.stress.principal.minimum.summary.min),

        safe_scalar(pp.strain.magnitude.summary.mean),
    ], dtype=np.float64)

    return y


def worker(sample_idx, seed):
    """
    One independent job:
    - create RNG
    - sample inputs
    - run FEA
    - validate output
    """
    rng = np.random.default_rng(seed)
    x, meta = sample_case(rng)
    y = run_one_fea_case(meta)

    if np.any(np.isnan(y)) or np.any(np.isinf(y)):
        raise ValueError("Invalid output values")

    return sample_idx, x, y, meta


def main():
    ctx = mp.get_context("spawn")  # safer than fork for many scientific stacks

    X_list = []
    Y_list = []
    kept_meta = []

    # Make independent seeds for each job
    master_ss = np.random.SeedSequence(MASTER_SEED)
    child_seeds = master_ss.spawn(N_SAMPLES)

    futures = []

    with ProcessPoolExecutor(max_workers=NUM_WORKERS, mp_context=ctx) as executor:
        for i in range(N_SAMPLES):
            seed = int(child_seeds[i].generate_state(1)[0])
            futures.append(executor.submit(worker, i, seed))

        completed = 0
        for future in as_completed(futures):
            completed += 1
            try:
                sample_idx, x, y, meta = future.result()
                X_list.append((sample_idx, x))
                Y_list.append((sample_idx, y))
                kept_meta.append((sample_idx, meta))

                print(f"[{completed}/{N_SAMPLES}] ok (sample {sample_idx + 1})", flush=True)

            except Exception as e:
                print(f"[{completed}/{N_SAMPLES}] failed: {e}", flush=True)
                traceback.print_exc()

    if len(X_list) == 0:
        raise RuntimeError("No valid samples were generated.")

    # Sort back into sample index order
    X_list.sort(key=lambda t: t[0])
    Y_list.sort(key=lambda t: t[0])
    kept_meta.sort(key=lambda t: t[0])

    X = np.vstack([x for _, x in X_list])
    Y = np.vstack([y for _, y in Y_list])
    meta = np.array([m for _, m in kept_meta], dtype=object)

    np.savez(
        OUT_FILE,
        X=X,
        Y=Y,
        meta=meta
    )

    print(f"Saved dataset to {OUT_FILE}")
    print("X shape:", X.shape)
    print("Y shape:", Y.shape)


if __name__ == "__main__":
    main()
