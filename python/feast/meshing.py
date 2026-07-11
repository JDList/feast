"""Meshing convenience helpers for FEAST."""

from __future__ import annotations

from ._core import StructuredTetMesher
from .geometry import build_cuboid_geometry


def mesh_cuboid(lx: float, ly: float, lz: float, element_size: float):
    """Build geometry and generate a structured Tet4 mesh for a cuboid."""
    geometry = build_cuboid_geometry(lx, ly, lz)
    return StructuredTetMesher(element_size).generate(geometry)


def mesh_unit_cuboid(element_size: float):
    """Mesh a 1 x 1 x 1 cuboid with the requested target element size."""
    return mesh_cuboid(1.0, 1.0, 1.0, element_size)
