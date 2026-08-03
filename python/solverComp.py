
from time import perf_counter

import feast

# ---------------------------------------------------------------------

LX = 2.0
LY = 1.0
LZ = 0.5

E = 210e9
NU = 0.3

sizes = [
    1.0,
    0.5,
    0.25,
    0.125,
    0.05,
]

print(
    f"{'h':>8}"
    f"{'Nodes':>10}"
    f"{'Elements':>12}"
    f"{'Mesh':>12}"
    f"{'Ke':>12}"
    f"{'     Solve (D|CG)':>12}"
    f"{'Post':>12}"
)

print("-" * 90)

for h in sizes:

    total_start = perf_counter()

    ###################################################################
    # Geometry
    ###################################################################

    cube = feast.Cuboid(LX, LY, LZ)

    geometry = feast.CuboidBuilder().build(cube)

    ###################################################################
    # Meshing
    ###################################################################

    t0 = perf_counter()

    mesher = feast.StructuredTetMesher(h)

    build = mesher.generate(geometry)

    mesh_time = perf_counter() - t0

    mesh = build.mesh

    ###################################################################
    # BCs
    ###################################################################

    region_bcs = feast.RegionBoundaryConditionSet()

    #for dof in (0, 1, 2):
    #    region_bcs.addRegionDirichlet("lowx", dof, 0)

    region_bcs.addRegionNeumann("highx", 0, 1e6)
    region_bcs.addRegionNeumann("lowx", 0, -1e6)

    bcs = feast.BoundaryConditionResolver.resolve(
        region_bcs,
        build,
    )

    ###################################################################
    # DOF map
    ###################################################################

    dof_map = feast.DofMap()

    dof_map.resize(
        len(mesh.nodes()),
        3,
    )

    ###################################################################
    # Material
    ###################################################################

    materials = [
        feast.LinearElastic(E, NU)
    ]

    ###################################################################
    # Element stiffness
    ###################################################################

    t0 = perf_counter()

    element_stiffnesses = []

    for element in mesh.elements():

        material = materials[element.material_id]

        element_stiffnesses.append(
            feast.Tet4.stiffnessMatrix(
                mesh,
                element,
                material,
            )
        )

    ke_time = perf_counter() - t0

    ###################################################################
    # Solve
    ###################################################################

    solverCG = feast.EigenCGSolver(
    tolerance=1e-10,
    max_iterations=5000,
    )
    solverD = feast.EigenDirectSolver()


    kernelCG = feast.Kernel(solverCG)
    kernelD = feast.Kernel(solverD)

    startCG = perf_counter()
    resultCG = kernelCG.solveLinearStatic(
        mesh,
        dof_map,
        bcs,
        element_stiffnesses,
    )
    solve_timeCG = perf_counter() - startCG
    print("Solve time CG:", solve_timeCG)

    startD = perf_counter()
    resultD = kernelD.solveLinearStatic(
        mesh,
        dof_map,
        bcs,
        element_stiffnesses,
    )
    solve_timeD = perf_counter() - startD
    print("Solve time Direct:", solve_timeD)

    ###################################################################
    # Postprocess
    ###################################################################

    t0 = perf_counter()

    feast.PostProcessor.process(
        resultD,
        mesh,
        dof_map,
        materials,
    )

    post_time = perf_counter() - t0

    ###################################################################
    # Print
    ###################################################################


    print(
        f"{h:8.3f}"
        f"{len(mesh.nodes()):10d}"
        f"{len(mesh.elements()):12d}"
        f"{mesh_time:12.4f}"
        f"{ke_time:12.4f}"
        f"{solve_timeD:12.4f}|{solve_timeCG:.4f}"
        f"{post_time:12.4f}"
    )
