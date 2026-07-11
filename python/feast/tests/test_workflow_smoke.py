import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root))

import feast

geometry = feast.build_unit_cuboid_geometry()
build = feast.mesh_unit_cuboid(1.0)

region_bcs = feast.make_region_boundary_conditions()
feast.clamp_region(region_bcs, "lowx")
feast.load_region(region_bcs, "highz", 2, -1000.0)

node_bcs = feast.resolve_region_boundary_conditions(region_bcs, build)

assert len(geometry.vertices()) == 8
assert len(geometry.surfaces()) == 6
assert node_bcs.numDirichlet() >= 3
assert node_bcs.numNeumann() >= 1

print("workflow ok")
