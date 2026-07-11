"""Boundary-condition convenience helpers for FEAST."""

from __future__ import annotations

from collections.abc import Iterable

from ._core import BoundaryConditionResolver, RegionBoundaryConditionSet


def canonical_face_names() -> tuple[str, ...]:
    """Return the six canonical cuboid face names."""
    return ("lowx", "highx", "lowy", "highy", "lowz", "highz")


def make_region_boundary_conditions(
    dirichlet: Iterable[tuple[str, int, float]] | None = None,
    neumann: Iterable[tuple[str, int, float]] | None = None,
) -> RegionBoundaryConditionSet:
    """Build a RegionBoundaryConditionSet from iterable specs.

    Each Dirichlet item is (region_name, dof, value).
    Each Neumann item is (region_name, dof, value).
    """
    region_bcs = RegionBoundaryConditionSet()

    for region_name, dof, value in dirichlet or ():
        region_bcs.addRegionDirichlet(region_name, dof, value)

    for region_name, dof, value in neumann or ():
        region_bcs.addRegionNeumann(region_name, dof, value)

    return region_bcs


def clamp_region(
    region_bcs: RegionBoundaryConditionSet,
    region_name: str,
    value: float = 0.0,
    dofs: Iterable[int] = (0, 1, 2),
) -> RegionBoundaryConditionSet:
    """Add Dirichlet conditions for the requested displacement DOFs."""
    for dof in dofs:
        region_bcs.addRegionDirichlet(region_name, dof, value)
    return region_bcs


def load_region(
    region_bcs: RegionBoundaryConditionSet,
    region_name: str,
    dof: int,
    value: float,
) -> RegionBoundaryConditionSet:
    """Add a region Neumann condition, usually for pressure or traction."""
    region_bcs.addRegionNeumann(region_name, dof, value)
    return region_bcs


def resolve_dirichlet_boundary_conditions(region_bcs, build_result):
    """Resolve only Dirichlet region conditions to node-based conditions."""
    return BoundaryConditionResolver.resolveDirichlet(region_bcs, build_result)


def resolve_neumann_boundary_conditions(region_bcs, build_result):
    """Resolve only Neumann region conditions to node-based conditions."""
    return BoundaryConditionResolver.resolveNeumann(region_bcs, build_result)


def resolve_region_boundary_conditions(region_bcs, build_result):
    """Resolve all region-based boundary conditions to node-based conditions."""
    return BoundaryConditionResolver.resolve(region_bcs, build_result)
