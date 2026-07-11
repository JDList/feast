"""FEAST Python interface.

This package re-exports the compiled C++ extension and adds a few thin
convenience functions for common geometry, meshing, and boundary-condition
workflows.
"""

from ._core import *  # noqa: F401,F403

from .geometry import build_cuboid_geometry, build_unit_cuboid_geometry
from .meshing import mesh_cuboid, mesh_unit_cuboid
from .boundary_conditions import (
    canonical_face_names,
    make_region_boundary_conditions,
    clamp_region,
    load_region,
    resolve_region_boundary_conditions,
    resolve_dirichlet_boundary_conditions,
    resolve_neumann_boundary_conditions,
)

__all__ = [name for name in globals() if not name.startswith("_")]
