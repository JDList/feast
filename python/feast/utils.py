"""Utility helpers for FEAST."""

from __future__ import annotations


def as_tuple3(values):
    """Convert a 3-item iterable into a tuple of three floats."""
    items = tuple(values)
    if len(items) != 3:
        raise ValueError("Expected exactly three values.")
    return float(items[0]), float(items[1]), float(items[2])
