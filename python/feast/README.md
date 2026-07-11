# FEAST Python package

This package is a Python-facing scaffold for **Finite Element Analysis, Simulations and Training**.

It gives you:

- geometry primitives and a cuboid builder,
- structured tetra meshing for cuboid-like geometry,
- named node and face groups,
- region-based boundary condition resolution,
- a clean hook for a future compiled C++ backend.

## Install locally

```bash
pip install -e .
```

## Quick example

```python
from feast import Cuboid, CuboidBuilder, StructuredTetMesher, RegionBoundaryConditionSet, BoundaryConditionResolver

cuboid = Cuboid(1.0, 1.0, 1.0)
geometry = CuboidBuilder().build(cuboid)
mesh_result = StructuredTetMesher(1.0).generate(geometry)

region_bcs = RegionBoundaryConditionSet()
region_bcs.add_region_dirichlet("lowx", 0, 0.0)
region_bcs.add_region_dirichlet("lowx", 1, 0.0)
region_bcs.add_region_dirichlet("lowx", 2, 0.0)
region_bcs.add_region_neumann("highz", 2, -1000.0)

node_bcs = BoundaryConditionResolver.resolve(region_bcs, mesh_result)
print(node_bcs.num_dirichlet(), node_bcs.num_neumann())
```

## Backend note

The pure Python package is usable right away for preprocessing and BC resolution.
Solver and postprocessing can later be wired to a compiled extension module named `feast._core`.
