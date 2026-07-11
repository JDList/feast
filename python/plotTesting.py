import numpy as np
import pyvista as pv

import feast


###############################################################################
# Build model
###############################################################################

lx = 2.0
ly = 1.0
lz = 0.5

element_size = 0.5

cube = feast.Cuboid(lx, ly, lz)

geometry = feast.CuboidBuilder().build(cube)

mesher = feast.StructuredTetMesher(element_size)

build = mesher.generate(geometry)

mesh = build.mesh


###############################################################################
# Boundary conditions
###############################################################################

region_bcs = feast.RegionBoundaryConditionSet()

for dof in (0, 1, 2):
    region_bcs.addRegionDirichlet("lowx", dof, 0.0)

region_bcs.addRegionNeumann("highx", 2, 1.0e6)

bcs = feast.BoundaryConditionResolver.resolve(region_bcs, build)


###############################################################################
# Materials
###############################################################################

materials = [
    feast.LinearElastic(
        210e9,
        0.30
    )
]


###############################################################################
# DOF map
###############################################################################

dof_map = feast.DofMap()

dof_map.resize(
    len(mesh.nodes()),
    3
)


###############################################################################
# Element stiffness
###############################################################################

element_stiffnesses = []

for element in mesh.elements():

    material = materials[element.material_id]

    Ke = feast.Tet4.stiffnessMatrix(
        mesh,
        element,
        material
    )

    element_stiffnesses.append(Ke)


###############################################################################
# Solve
###############################################################################

solver = feast.EigenDirectSolver()

kernel = feast.Kernel(solver)

result = kernel.solveLinearStatic(
    mesh,
    dof_map,
    bcs,
    element_stiffnesses
)


###############################################################################
# Postprocessing
###############################################################################

pp = feast.PostProcessor.process(
    result,
    mesh,
    dof_map,
    materials
)


###############################################################################
# Extract coordinates
###############################################################################

coords = np.array([
    [n.x, n.y, n.z]
    for n in mesh.nodes()
])


###############################################################################
# Extract displacements
###############################################################################

disp = np.array(pp.displacement.values)

print(coords.shape)
print(disp.shape)


###############################################################################
# Scale deformation
###############################################################################

scale = 5000.0

coords_def = coords + scale * disp


###############################################################################
# Visualisation
###############################################################################

plotter = pv.Plotter()

plotter.add_points(
    coords,
    color="blue",
    point_size=12,
    render_points_as_spheres=True,
    label="Original"
)

plotter.add_points(
    coords_def,
    color="red",
    point_size=12,
    render_points_as_spheres=True,
    label="Deformed"
)


###############################################################################
# Draw displacement vectors
###############################################################################

for p0, p1 in zip(coords, coords_def):

    plotter.add_lines(
        np.vstack((p0, p1)),
        color="black",
        width=1
    )


plotter.add_axes()

plotter.show_grid()

plotter.add_legend()

plotter.show()
