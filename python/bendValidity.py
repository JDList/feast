"""
Validate FEAST cantilever bending against Euler-Bernoulli beam theory.

The bar extends along +x, is fixed on ``lowx``, and is loaded in the z
direction on ``highx``. The main validation quantity is average loaded-face
z-displacement, compared with

    delta = P L^3 / (3 E I),    I = W H^3 / 12.

Set LOAD_MODE correctly:
- ``surface_traction``: LOAD_VALUE is force per unit area.
- ``total_force``: LOAD_VALUE is the total force applied to the face.
"""

from __future__ import annotations

from dataclasses import dataclass
from math import log
from time import perf_counter

import feast


# Model configuration -------------------------------------------------------
LENGTH = 10.0
WIDTH = 5.0
HEIGHT = 1.0

YOUNGS_MODULUS = 210.0e9
POISSON_RATIO = 0.30

LOAD_VALUE = -10e6
LOAD_MODE = "surface_traction"  
LOAD_DOF = 2                    # z displacement/force

MESH_SIZES = (1.0, 0.5, 0.25, 0.125, 0.05)
FINEST_RELATIVE_ERROR_LIMIT = 0.15
REQUIRE_MONOTONIC_CONVERGENCE = True
USE_DIRECT_SOLVER = False


@dataclass(frozen=True)
class Row:
    h: float
    nodes: int
    elements: int
    dofs: int
    analytical_tip: float
    feast_tip: float
    relative_error: float
    mesh_time: float
    ke_time: float
    solve_time: float
    post_time: float
    total_time: float
    cg_iterations: int
    cg_error: float


def total_force() -> float:
    if LOAD_MODE == "surface_traction":
        return LOAD_VALUE * WIDTH * HEIGHT
    if LOAD_MODE == "total_force":
        return LOAD_VALUE
    raise ValueError("LOAD_MODE must be 'surface_traction' or 'total_force'.")


def analytical_tip_displacement() -> float:
    inertia = WIDTH * HEIGHT**3 / 12.0
    return total_force() * LENGTH**3 / (3.0 * YOUNGS_MODULUS * inertia)


def global_dof(dof_map, node_index: int, component: int) -> int:
    for name in ("dof", "globalDof", "nodeDof"):
        fn = getattr(dof_map, name, None)
        if fn is not None:
            return int(fn(node_index, component))
    return 3 * node_index + component


def highx_node_indices(build, mesh) -> list[int]:
    node_sets = build.groups.node_sets
    ids = [int(value) for value in node_sets["highx"]]
    if not ids:
        raise RuntimeError("The highx node set is empty.")

    node_count = len(mesh.nodes())
    if all(0 <= value < node_count for value in ids):
        return ids

    # Fallback for bindings exposing node IDs that are not container indices.
    id_to_index = {}
    for index, node in enumerate(mesh.nodes()):
        id_to_index[int(getattr(node, "id", index))] = index
    return [id_to_index[value] for value in ids]


def average_tip_displacement(result, dof_map, tip_nodes: list[int]) -> float:
    values = [
        float(result.solution[global_dof(dof_map, node, LOAD_DOF)])
        for node in tip_nodes
    ]
    return sum(values) / len(values)


def build_stiffnesses(mesh, material):
    builder = getattr(feast, "ElementMatrixBuilder", None)
    if builder is not None:
        return builder.buildStiffnesses(mesh, material)

    materials = [material]
    return [
        feast.Tet4.stiffnessMatrix(
            mesh,
            element,
            materials[element.material_id],
        )
        for element in mesh.elements()
    ]


def run_case(h: float) -> Row:
    total_start = perf_counter()

    start = perf_counter()
    cuboid = feast.Cuboid(LENGTH, WIDTH, HEIGHT)
    geometry = feast.CuboidBuilder().build(cuboid)
    build = feast.StructuredTetMesher(h).generate(geometry)
    mesh = build.mesh
    mesh_time = perf_counter() - start

    region_bcs = feast.RegionBoundaryConditionSet()
    for dof in (0, 1, 2):
        region_bcs.addRegionDirichlet("lowx", dof, 0.0)
    region_bcs.addRegionNeumann("highx", LOAD_DOF, LOAD_VALUE)

    resolved_bcs = feast.BoundaryConditionResolver.resolve(region_bcs, build)

    dof_map = feast.DofMap()
    dof_map.resize(len(mesh.nodes()), 3)

    material = feast.LinearElastic(YOUNGS_MODULUS, POISSON_RATIO)
    materials = [material]

    start = perf_counter()
    #element_stiffnesses = build_stiffnesses(mesh, material)
    ke_time = perf_counter() - start
    if USE_DIRECT_SOLVER:
        solver = feast.EigenDirectSolver()
    else:
        solver = feast.EigenCGSolver(
         tolerance=1e-10,
         max_iterations=20000,
         )  
    kernel = feast.Kernel(solver)

    start = perf_counter()
    result = kernel.solveLinearStatic(
        mesh,
        dof_map,
        resolved_bcs,
        materials
    )
    solve_time = perf_counter() - start

    start = perf_counter()
    feast.PostProcessor.process(result, mesh, dof_map, materials)
    post_time = perf_counter() - start

    theory = analytical_tip_displacement()
    tip = average_tip_displacement(
        result,
        dof_map,
        highx_node_indices(build, mesh),
    )
    error = abs(tip - theory) / abs(theory)

    iterations = int(getattr(solver, "iterations", 0))
    estimated_error = float(
        getattr(solver, "estimated_error", 0.0)
    )

    return Row(
        h=h,
        nodes=len(mesh.nodes()),
        elements=len(mesh.elements()),
        dofs=result.solution.size(),
        analytical_tip=theory,
        feast_tip=tip,
        relative_error=error,
        mesh_time=mesh_time,
        ke_time=ke_time,
        solve_time=solve_time,
        post_time=post_time,
        total_time=perf_counter() - total_start,
        cg_iterations=iterations,
        cg_error=estimated_error,
    )


def convergence_orders(rows: list[Row]) -> list[float | None]:
    orders: list[float | None] = [None]
    for coarse, fine in zip(rows, rows[1:]):
        if coarse.relative_error <= 0.0 or fine.relative_error <= 0.0:
            orders.append(None)
        else:
            orders.append(
                log(coarse.relative_error / fine.relative_error)
                / log(coarse.h / fine.h)
            )
    return orders


def print_results(rows: list[Row]) -> None:
    orders = convergence_orders(rows)
    print("\nFEAST cantilever bending validation")
    print(f"Total force: {total_force():.8e}")
    print(f"Euler-Bernoulli tip displacement: {rows[0].analytical_tip:.12e}\n")

    header = (
        f"{'h':>8} {'Nodes':>9} {'Elements':>10} "
        f"{'FEAST tip':>15} {'Theory':>15} {'Rel.err':>10} "
        f"{'Order':>8} {'CG it.':>8} {'Total(s)':>10}"
    )
    print(header)
    print("-" * len(header))

    for row, order in zip(rows, orders):
        order_text = "-" if order is None else f"{order:.3f}"
        print(
            f"{row.h:8.4f} {row.nodes:9d} {row.elements:10d} "
            f"{row.feast_tip:15.7e} {row.analytical_tip:15.7e} "
            f"{row.relative_error:10.3%} {order_text:>8} "
            f"{row.cg_iterations:8d} {row.total_time:10.4f}"
        )

    print("\nStage timings")
    print(f"{'h':>8} {'Mesh':>10} {'Ke':>10} {'Solve':>10} {'Post':>10} {'CG error':>13}")
    print("-" * 65)
    for row in rows:
        print(
            f"{row.h:8.4f} {row.mesh_time:10.4f} {row.ke_time:10.4f} "
            f"{row.solve_time:10.4f} {row.post_time:10.4f} "
            f"{row.cg_error:13.3e}"
        )


def validate(rows: list[Row]) -> None:
    failures: list[str] = []

    for row in rows:
        if row.feast_tip == 0.0:
            failures.append(f"h={row.h:g}: tip displacement is zero.")
        if row.feast_tip * row.analytical_tip <= 0.0:
            failures.append(
                f"h={row.h:g}: FEAST and theoretical displacement signs differ."
            )

    if REQUIRE_MONOTONIC_CONVERGENCE:
        for coarse, fine in zip(rows, rows[1:]):
            if fine.relative_error > coarse.relative_error + 1.0e-12:
                failures.append(
                    f"Error increased from h={coarse.h:g} "
                    f"({coarse.relative_error:.3%}) to h={fine.h:g} "
                    f"({fine.relative_error:.3%})."
                )

    if rows[-1].relative_error > FINEST_RELATIVE_ERROR_LIMIT:
        failures.append(
            f"Finest error {rows[-1].relative_error:.3%} exceeds "
            f"limit {FINEST_RELATIVE_ERROR_LIMIT:.3%}."
        )

    if failures:
        print("\nVALIDATION FAILED")
        for failure in failures:
            print(f"  - {failure}")
        raise SystemExit(1)

    print("\nVALIDATION PASSED")
    print(f"Finest relative error: {rows[-1].relative_error:.3%}")


def main() -> None:
    rows = [run_case(h) for h in MESH_SIZES]
    print_results(rows)
    validate(rows)


if __name__ == "__main__":
    main()
