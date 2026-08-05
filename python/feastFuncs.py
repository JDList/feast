
import feast

def getCuboid(Lx, Ly, Lz):
    return feast.CuboidBuilder().build(feast.Cuboid(Lx,Ly,Lz))

def buildMesh(Cuboid, h = 0.1):
    return feast.StructuredTetMesher(h).generate(Cuboid)

def fixRegion(regionBcs, region):
    for dof in (0,1,2):
        regionBcs.addRegionDirichlet(region, dof, 0.0)
    return regionBcs

def getPP(mesh, resolved_bcs, material, useCG = True):
    
    dof_map = feast.DofMap()
    dof_map.resize(len(mesh.nodes()), 3)
    if useCG:
        solver = feast.EigenCGSolver(tolerance=1e-10,max_iterations=5000)
    else:
        solver = feast.EigenDirectSolver()

    kernel = feast.Kernel(solver)

    result = kernel.solveLinearStatic( mesh,dof_map,resolved_bcs,[material])
            

    pp = feast.PostProcessor.process(
            result,
            mesh,
            dof_map,
            [material],
            )
    return pp



