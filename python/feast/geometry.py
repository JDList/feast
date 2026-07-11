"""Geometry convenience helpers for FEAST."""

from __future__ import annotations

from ._core import Cuboid, CuboidBuilder


def build_cuboid_geometry(lx: float, ly: float, lz: float):
    """Build a Geometry object from a Cuboid."""
    return CuboidBuilder().build(Cuboid(lx, ly, lz))


def build_unit_cuboid_geometry():
    """Build a 1 x 1 x 1 Geometry object."""
    return build_cuboid_geometry(1.0, 1.0, 1.0)
