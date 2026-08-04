"""
Basic feast run through to test debugging in cpp code
"""
import feast
from time import perf_counter
# Geometry
cuboid = feast.Cuboid(10.0, 5.0, 1.0)
geometry = feast.CuboidBuilder().build(cuboid)

# Meshing
mesher = feast.StructuredTetMesher(0.1)
build = mesher.generate(geometry)

mesh = build.mesh
groups = build.groups

print("mesh nodes:", len(mesh.nodes()))
print("mesh elements:", len(mesh.elements()))
print("node sets:", sorted(groups.node_sets.keys()))
print("face sets:", sorted(groups.face_sets.keys()))


# Boundary conditions
region_bcs = feast.RegionBoundaryConditionSet()
for dof in (0, 1, 2):
    region_bcs.addRegionDirichlet("lowx", dof, 0.0)
#region_bcs.addRegionNeumann("lowx", 0, 1000.0)
region_bcs.addRegionNeumann("highx", 0, -1000.0)

resolved_bcs = feast.BoundaryConditionResolver.resolve(region_bcs, build)

print("resolved dirichlet:", resolved_bcs.numDirichlet())
print("resolved neumann:", resolved_bcs.numNeumann())

# DOF map / materials / element stiffness
dof_map = feast.DofMap()
dof_map.resize(len(mesh.nodes()), 3)

material = feast.LinearElastic(210e7, 0.3)

beforeFor = perf_counter()

element_stiffnesses = feast.ElementMatrixBuilder.buildStiffnesses(mesh,material)

assert len(element_stiffnesses) == len(mesh.elements())

afterFor = perf_counter()
print(f"Material for loop time: {afterFor-beforeFor}")
# Solve
solver = feast.EigenCGSolver()
kernel = feast.Kernel(solver)

result = kernel.solveLinearStatic(
        mesh,
        dof_map,
        resolved_bcs,
        element_stiffnesses,
        )

print("solution size:", result.solution.size())

# Postprocessing
pp = feast.PostProcessor.process(
        result,
        mesh,
        dof_map,
        [material],
        )

print("max displacement magnitude:", pp.displacement.magnitude.summary.max)
print("max von Mises stress:", pp.stress.vonMises.summary.max)
print("max hydrostatic stress:", pp.stress.hydrostatic.summary.max)
print("max principal stress:", pp.stress.principal.maximum.summary.max)



