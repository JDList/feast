"""
FEAST end-to-end smoke test.

Run from the FEAST repo root with:

    PYTHONPATH=python python3 feast_full_pipeline_smoke_test.py

This exercises:
  geometry -> meshing -> region BCs -> solver -> postprocessing
"""

from __future__ import annotations

import math


def main() -> int:
    try:
        import feast
    except Exception as exc:
        print("Failed to import feast:", exc)
        return 1

    # Geometry
    cuboid = feast.Cuboid(1.0, 1.0, 1.0)
    geometry = feast.CuboidBuilder().build(cuboid)

    assert len(geometry.vertices()) == 8, f"expected 8 vertices, got {len(geometry.vertices())}"
    assert len(geometry.surfaces()) == 6, f"expected 6 surfaces, got {len(geometry.surfaces())}"

    # Meshing
    mesher = feast.StructuredTetMesher(0.15)
    build = mesher.generate(geometry)

    mesh = build.mesh
    groups = build.groups

    print("mesh nodes:", len(mesh.nodes()))
    print("mesh elements:", len(mesh.elements()))
    print("node sets:", sorted(groups.node_sets.keys()))
    print("face sets:", sorted(groups.face_sets.keys()))


    expected_regions = {"lowx", "highx", "lowy", "highy", "lowz", "highz"}
    assert expected_regions.issubset(set(groups.node_sets.keys())), (
        f"missing node sets: {expected_regions - set(groups.node_sets.keys())}"
    )
    assert expected_regions.issubset(set(groups.face_sets.keys())), (
        f"missing face sets: {expected_regions - set(groups.face_sets.keys())}"
    )

    # Boundary conditions
    region_bcs = feast.RegionBoundaryConditionSet()
    for dof in (0, 1, 2):
        region_bcs.addRegionDirichlet("lowx", dof, 0.0)
    region_bcs.addRegionNeumann("highx", 0, -1000.0)

    resolved_bcs = feast.BoundaryConditionResolver.resolve(region_bcs, build)

    print("resolved dirichlet:", resolved_bcs.numDirichlet())
    print("resolved neumann:", resolved_bcs.numNeumann())

    assert resolved_bcs.numDirichlet() > 0, "expected resolved Dirichlet conditions"
    assert resolved_bcs.numNeumann() > 0, "expected resolved Neumann conditions"

    # DOF map / materials / element stiffness
    dof_map = feast.DofMap()
    dof_map.resize(len(mesh.nodes()), 3)

    materials = [feast.LinearElastic(210e7, 0.3)]

    element_stiffnesses = []
    for element in mesh.elements():
        material = materials[element.material_id]
        element_stiffnesses.append(feast.Tet4.stiffnessMatrix(mesh, element, material))

    assert len(element_stiffnesses) == len(mesh.elements())

    # Solve
    solver = feast.EigenDirectSolver()
    kernel = feast.Kernel(solver)

    result = kernel.solveLinearStatic(
        mesh,
        dof_map,
        resolved_bcs,
        element_stiffnesses,
    )

    print("solution size:", result.solution.size())
    assert result.solution.size() == len(mesh.nodes()) * 3, (
        f"expected solution size {len(mesh.nodes()) * 3}, got {result.solution.size()}"
    )

    # Postprocessing
    pp = feast.PostProcessor.process(
        result,
        mesh,
        dof_map,
        materials,
    )

    print("max displacement magnitude:", pp.displacement.magnitude.summary.max)
    print("max von Mises stress:", pp.stress.vonMises.summary.max)
    print("max hydrostatic stress:", pp.stress.hydrostatic.summary.max)
    print("max principal stress:", pp.stress.principal.maximum.summary.max)

    assert math.isfinite(pp.displacement.magnitude.summary.max)
    assert math.isfinite(pp.stress.vonMises.summary.max)
    assert math.isfinite(pp.stress.hydrostatic.summary.max)
    assert math.isfinite(pp.stress.principal.maximum.summary.max)

    print("FEAST smoke test completed successfully.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
