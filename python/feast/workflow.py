"""Small FEAST workflow helpers."""

from __future__ import annotations

from .geometry import build_cuboid_geometry
from .meshing import mesh_cuboid
from .boundary_conditions import (
    canonical_face_names,
    clamp_region,
    load_region,
    make_region_boundary_conditions,
    resolve_region_boundary_conditions,
)


def build_cuboid_workflow(lx: float, ly: float, lz: float, element_size: float):
    """Build geometry and mesh for a cuboid in one call."""
    return mesh_cuboid(lx, ly, lz, element_size)


def build_unit_cuboid_workflow(element_size: float):
    """Build geometry and mesh for a unit cuboid in one call."""
    return mesh_cuboid(1.0, 1.0, 1.0, element_size)


def default_cuboid_bcs():
    """Create a common clamp/load example for cuboid-style problems."""
    region_bcs = make_region_boundary_conditions()
    clamp_region(region_bcs, "lowx")
    return region_bcs


__all__ = [
    "build_cuboid_geometry",
    "build_cuboid_workflow",
    "build_unit_cuboid_workflow",
    "canonical_face_names",
    "clamp_region",
    "default_cuboid_bcs",
    "load_region",
    "make_region_boundary_conditions",
    "resolve_region_boundary_conditions",
]
