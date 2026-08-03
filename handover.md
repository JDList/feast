# FEAST Project Handover

**Finite Element Analysis, Simulations and Training**

*C++ core, Python bindings, solver expansion, performance work, current status, and next priorities*

Prepared for continuation by a future assistant or contributor. Updated after iterative solver, constraint, and assembly optimisation work.

| Current development focus: Preserve the modular FE architecture while improving sparse assembly and constraint performance, validating the new conjugate-gradient solver, and preparing for richer geometry and element types. |
| --- |

## 1. Executive summary

FEAST is a modular C++ finite-element framework for structural mechanics. The C++ core remains authoritative for geometry, meshing, finite-element calculations, assembly, constraints, solving, and postprocessing. Python is the orchestration layer for scripting, visualization, parameter studies, convergence analysis, optimisation, and future machine-learning workflows.

| Area | Current decision / state |
| --- | --- |
| Architecture | Geometry -> meshing -> Mesh/MeshGroups -> region BC resolution -> node BCs -> assembly/constraints -> solver -> LinearStaticResult -> postprocessing. |
| Mesh policy | Mesh stores nodes and elements only. Named selections remain in MeshGroups. |
| Boundary conditions | High-level region conditions are resolved to node-level conditions before Kernel and ConstraintApplier. |
| Linear solvers | EigenDirectSolver remains the reference solver. EigenCGSolver has been added for SPD systems and exposes convergence diagnostics. |
| Performance | The original dominant cost was not only factorization. Profiling showed severe costs in sparse Dirichlet modification and vector growth during assembly; both have been substantially improved. |
| Backend isolation | Eigen usage should remain confined to linalg implementation files wherever practical. Assembly and constraints should use FEAST SparseMatrix and Triplet APIs. |
| Immediate priorities | Lock in regression tests and benchmarks, finish backend-neutral sparse APIs, validate CG against analytical cases, then add a cylinder and a suitable mesher path. |

| Most important recent finding: A timer around Kernel::solveLinearStatic included global assembly, constraint application, solver work, and result construction. Internal timing showed the direct or CG solver was not the only bottleneck. ConstraintApplier and GlobalAssembler required targeted sparse-data-structure optimisation. |
| --- |

## 2. Naming, philosophy, and non-negotiable decisions

The expanded project name is Finite Element Analysis, Simulations and Training. Existing namespace and include paths may continue to use feast and feast/...; renaming is not an immediate priority.

- C++ owns numerical computation and is the source of truth.
- Python owns orchestration, plotting, notebooks, studies, optimisation, and data workflows.
- Bindings remain thin: export C++ types and add convenience functions rather than parallel Python implementations.
- Mesh remains pure: no named groups, postprocessing fields, or solver state.
- Kernel remains unaware of high-level region conditions.
- Tet4 retains formulation logic for stiffness, strain, and stress recovery; PostProcessor orchestrates rather than duplicates it.
- Eigen stays inside linalg wherever practical. Assembly and constraints use FEAST abstractions.
- Optimisations must preserve correctness, symmetry, sparse structure, and future support for mixed element types.

## 3. Current architecture and data flow

```text
Primitive geometry
    -> Builder
    -> Geometry
    -> Mesher
    -> MeshBuildResult
         |-- Mesh
         `-- MeshGroups (node_sets, face_sets)
    -> RegionBoundaryConditionSet
    -> BoundaryConditionResolver
    -> BoundaryConditionSet
    -> DofMap + element matrices/vectors
    -> GlobalAssembler
    -> SparseMatrix K + Vector f
    -> ConstraintApplier
    -> LinearSolver
    -> LinearStaticResult
    -> PostProcessor
    -> PostProcessResult
```

### 3.1 Current intended user workflow

```cpp
feast::Cuboid cuboid(1.0, 1.0, 1.0);
feast::Geometry geometry = feast::CuboidBuilder().build(cuboid);
feast::MeshBuildResult build = feast::StructuredTetMesher(0.25).generate(geometry);

feast::RegionBoundaryConditionSet regionBCs;
regionBCs.addRegionDirichlet("lowx", 0, 0.0);
regionBCs.addRegionDirichlet("lowx", 1, 0.0);
regionBCs.addRegionDirichlet("lowx", 2, 0.0);
regionBCs.addRegionNeumann("highx", 0, 1000.0);

feast::BoundaryConditionSet bcs =
    feast::BoundaryConditionResolver::resolve(regionBCs, build);

feast::LinearStaticResult result =
    kernel.solveLinearStatic(build.mesh, dofMap, bcs,
                             elementStiffnesses, elementVectors);

feast::PostProcessResult pp =
    feast::PostProcessor::process(result, build.mesh, dofMap, materials);
```

## 4. Confirmed C++ API direction

### 4.1 Linear algebra and solver ownership

- Vector wraps Eigen::VectorXd; DenseMatrix wraps Eigen::MatrixXd; SparseMatrix wraps Eigen::SparseMatrix<double>.
- Assembly emits FEAST Triplet records. Conversion to Eigen triplets happens inside SparseMatrix::setFromTriplets.
- LinearSolver is the backend-neutral solver interface.
- EigenDirectSolver and EigenCGSolver are concrete implementations under linalg.
- Kernel owns a const LinearSolver& and therefore has no default constructor.
- Python Kernel binding must retain py::keep_alive<1, 2>() so the solver outlives Kernel.

```cpp
class LinearSolver {
public:
    virtual ~LinearSolver() = default;
    virtual Vector solve(const SparseMatrix& K,
                         const Vector& f) const = 0;
};
```

### 4.2 SparseMatrix boundary

The current public SparseMatrix header exposes Eigen directly, but recent work added or recommended backend-neutral operations so assembly and constraints no longer need to call eigen(). The practical near-term boundary is:

| Layer | Permitted sparse API |
| --- | --- |
| assembly / constraints | rows(), cols(), nonZeros(), setFromTriplets(), forEachNonZero(), FEAST Triplet |
| Eigen-specific linalg | eigen() may be used by SparseMatrix.cpp, EigenDirectSolver.cpp, and EigenCGSolver.cpp |
| future cleanup | Move Eigen access to an internal adapter or PIMPL once APIs stabilise |

## 5. EigenCGSolver implementation and integration

EigenCGSolver was introduced as the first iterative solver. It uses Eigen::ConjugateGradient with diagonal preconditioning, preserves the existing LinearSolver interface, and stores diagnostics from the latest solve.

| Property | Behaviour |
| --- | --- |
| tolerance | Configurable; default used during development: 1e-10. |
| maximum iterations | Configurable; typical tests used 1,000-5,000. |
| diagnostics | converged, iterations, estimatedError. |
| failure policy | Throw if setup fails or the solver does not converge. |
| matrix requirement | The constrained matrix must be symmetric positive definite. |
| result conversion | Eigen::VectorXd must be copied into feast::Vector via resize and indexed assignment; Vector has no Eigen constructor. |

### 5.1 Header and include facts

- LinearSolver.hpp lives under feast/solvers/LinearSolver.hpp, not feast/linalg/LinearSolver.hpp.
- EigenCGSolver.hpp is appropriately located under feast/linalg/.
- The source must be added to the core feast library target.
- bindings/linalg.cpp must include and bind EigenCGSolver.
- python/feast/__init__.py needs no change when it uses from ._core import *.

### 5.2 Python use

```python
solver = feast.EigenCGSolver(
    tolerance=1e-10,
    max_iterations=5000,
)
kernel = feast.Kernel(solver)
result = kernel.solveLinearStatic(mesh, dof_map, bcs, element_stiffnesses)

print(solver.converged)
print(solver.iterations)
print(solver.estimated_error)
```

### 5.3 Binding verification

```python
PYTHONPATH=python python3 - <<'PY'
import feast._core as core
print(core.__file__)
print([name for name in dir(core) if "Solver" in name])
PY
```

Expected symbols include EigenCGSolver, EigenDirectSolver, and LinearSolver. If EigenCGSolver is absent, the local extension is being loaded but bindings/linalg.cpp or the _core rebuild is incomplete.

## 6. Performance profiling and findings

The project originally reported severe scaling in the overall solve stage. Detailed internal timing was then added around assembly, constraints, solver execution, and result construction.

| Stage | Representative measured time | Interpretation |
| --- | --- | --- |
| Assembly | 3.419 s | Substantial; later became the main bottleneck after constraint optimisation. |
| Constraints | 11.039 s | Originally dominant due to repeated random sparse edits using coeffRef. |
| CG solver | 0.967 s | Not the dominant cost for the profiled 1,536-DOF model. |
| Result construction | 0.0006 s | Negligible. |

Representative model: 512 nodes, 2,058 Tet4 elements, 1,536 displacement DOFs, 192 resolved Dirichlet conditions, and 64 resolved Neumann conditions. The CG solver converged, indicating the constrained system was numerically suitable for CG in this case.

## 7. ConstraintApplier: root cause and corrected approach

The original implementation looped over every row and column for every constrained DOF and used Eigen coeffRef to assign zeros. In a sparse matrix, coeffRef may insert entries that do not already exist. This caused repeated sparse searches, reallocations, explicit stored zeros, and loss of sparse efficiency.

```text
for each constrained DOF g:
    for every row i: read A(i,g) and adjust f
    for every column j: A(g,j) = 0 using coeffRef
    for every row i:    A(i,g) = 0 using coeffRef
    A(g,g) = 1
```

The improved method rebuilds the constrained matrix once:

1. Build a global-DOF-to-prescribed-value map and detect conflicting duplicates.
1. Create compact constrained flags and prescribed-value arrays.
1. Traverse only existing nonzero entries through SparseMatrix::forEachNonZero.
1. Discard constrained rows.
1. For constrained columns, move K(i,g) * u_g to the right-hand side and omit the entry.
1. Keep all unconstrained entries as FEAST Triplet records.
1. Add one unit diagonal entry per constrained DOF and set the prescribed force entry.
1. Replace the matrix once through SparseMatrix::setFromTriplets, which compresses the backend representation.

| Correctness requirement: For nonzero prescribed displacements, the RHS adjustment must occur before constrained columns are removed. Removing both rows and columns preserves symmetry, which is essential for CG. |
| --- |

## 8. Global assembly: root cause and optimisation

The original ElementAssembler called triplets.reserve(triplets.size() + n*n) once per element. Although intended as an optimisation, this can force the vector to grow in small element-sized increments, repeatedly reallocating and moving the entire accumulated triplet array.

For 2,058 Tet4 elements, up to 296,352 local contributions may be emitted before duplicate global entries are combined. Reserving in small increments can approach quadratic total data movement.

| Change | Why it matters |
| --- | --- |
| Precompute expected triplets | Sum rows*cols across element stiffness matrices; works for Tet4, Hex8, beams, shells, and mixed element sizes. |
| Reserve once in GlobalAssembler | Allocates one contiguous block before the hot loop and avoids repeated moves. |
| Remove per-element reserve | Allows the single global reservation to govern capacity. |
| Validate DOF IDs once | Avoid repeated negative-index checks inside the n x n loop. |
| Cache mesh.elements() reference | Avoid repeated accessor calls and clearly prevent copies when the accessor returns const reference. |
| Construct SparseMatrix at final size | Avoid redundant resize/setZero before setFromTriplets. |
| Move into AssemblyResult | Avoid copying the sparse matrix and force vector. |
| Reuse element DOF storage later | An output-parameter or span-based DofMap API can avoid one small allocation per element. |

| Generalisation: Do not hard-code 12 x 12 in the generic assembler. Reserve using each element stiffness matrix dimensions so future Tet4, Hex8, beam, shell, and mixed-element workflows remain supported. |
| --- |

## 9. API and implementation checks

### 9.1 Confirm container accessors return references

Mesh::elements() should be declared and defined as a const reference accessor:

```cpp
const std::vector<Element>& elements() const;

const std::vector<Element>& Mesh::elements() const
{
    return m_elements;
}
```

Using const auto& elements = mesh.elements(); does not by itself prove no copy occurred: if elements() returns by value, the reference merely extends the temporary lifetime. A compile-time assertion can enforce the contract.

```cpp
static_assert(
    std::is_same_v<
        decltype(std::declval<const feast::Mesh&>().elements()),
        const std::vector<feast::Element>&
    >,
    "Mesh::elements() should return const std::vector<Element>&"
);
```

### 9.2 Release builds

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure

grep CMAKE_BUILD_TYPE build/CMakeCache.txt
```

## 10. Current numerical and postprocessing state

| Quantity | Representative output |
| --- | --- |
| Nodes | 512 |
| Tet4 elements | 2,058 |
| Resolved Dirichlet conditions | 192 |
| Resolved Neumann conditions | 64 |
| Solution size | 1,536 |
| Maximum displacement magnitude | 4.8725765e-07 |
| Maximum von Mises stress | 1215.8435 |
| Maximum hydrostatic stress | -283.5765 |
| Maximum principal stress | 27.1952 |

The negative hydrostatic value may be valid depending on the sign convention. The maximum-principal summary should be checked to confirm whether it means maximum algebraic principal stress, maximum absolute principal stress, or a particular principal component. Preserve explicit postprocessing fields and document sign conventions.

## 11. Geometry, meshing, and Python status

- Cuboid remains a parameter holder; CuboidBuilder owns topology and canonical face naming.
- StructuredTetMesher remains a specialised axis-aligned cuboid mesher and should not be expanded into a universal mesher.
- MeshBuildResult contains Mesh and MeshGroups.
- Canonical cuboid region names remain lowx, highx, lowy, highy, lowz, and highz.
- Region Neumann conditions are traction-like and are converted to area-weighted equivalent nodal loads.
- Python exposes direct C++ types through _core and should add convenience functions rather than duplicate classes.
- High-value Python modules remain extraction.py, visualization.py, convergence.py, benchmark.py, export.py, and studies.py.

## 12. Verification and regression plan

| Area | Required tests |
| --- | --- |
| EigenCGSolver | Known SPD systems; convergence metadata; invalid tolerance/iteration arguments; direct-vs-CG solution comparison. |
| ConstraintApplier | Zero and nonzero prescribed displacement; duplicate/conflicting constraints; symmetry preservation; expected nonzero count reduction; no explicit-zero explosion. |
| GlobalAssembler | Mixed element matrix sizes; duplicate triplet summation; vector contributions; Neumann accumulation; no copies of mesh containers. |
| SparseMatrix | Triplet replacement; duplicate summation; nonzero traversal; row/column bounds; backend compression invariant. |
| Structural validation | Uniaxial extension, pure shear, hydrostatic loading, cantilever bending, mesh convergence, total force equilibrium. |
| Performance regression | Stage timings, CG iterations, matrix nonzeros before/after constraints, and release-build benchmark history. |

## 13. Recommended next steps

### 13.1 Immediate: stabilise performance changes

1. Commit EigenCGSolver, bindings, CMake changes, backend-neutral SparseMatrix additions, ConstraintApplier rebuild logic, and GlobalAssembler reservation/move changes together with tests.
1. Add permanent stage timing support or a benchmark-only instrumentation path rather than relying on ad hoc std::cout statements.
1. Record matrix dimensions, nonzero counts before constraints, nonzero counts after constraints, CG iterations, residual estimate, and timing per stage.
1. Add a benchmark threshold or trend report so accidental return of per-element reserve or coeffRef row/column edits is detected.

### 13.2 Short term: improve iterative solving

1. Compare diagonal preconditioning with Eigen IncompleteCholesky on the same constrained systems.
1. Add a solver information/result design when diagnostic requirements outgrow mutable last-solve fields.
1. Consider EigenBiCGSTAB only for genuinely nonsymmetric systems; keep CG for validated SPD elasticity systems.
1. Check behaviour for near-incompressible materials and badly shaped elements, where conditioning may dominate iteration count.

### 13.3 Next feature milestone: cylinder geometry

After correctness and performance regression tests are secure, add a cylinder as the first non-cuboid geometry. It forces the project to move beyond six axis-aligned faces while remaining understandable enough for validation.

1. Add a small Cylinder parameter class.
1. Add CylinderBuilder with named top, bottom, and lateral surfaces.
1. Decide between a specialised structured cylinder mesher and an adapter to an external tetrahedral mesher such as Gmsh.
1. Map external physical groups or internal named surfaces back into MeshGroups node_sets and face_sets.
1. Add axial loading, torsion, and pressure-style validation examples.

### 13.4 Medium term: element and DOF extensibility

- Keep ElementAssembler generic over arbitrary n x n element matrices.
- Adopt std::span<const int> or a reusable output buffer for element DOF IDs to support fixed and runtime-sized elements without per-element allocations.
- Allow element formulations to declare their DOF layout; do not assume every element uses three translational DOFs per node.
- Prepare for Hex8, beams, plates, or shells only after validation infrastructure and result schemas are stable.

## 14. Known limitations and technical debt

- StructuredTetMesher is cuboid-like and axis-aligned.
- BoundaryFaceCollector supports Tet4 only.
- Region Neumann loading assumes constant traction and equal distribution over triangular face nodes.
- No point-selector or point-BC system exists.
- Python packaging remains source-tree oriented unless pyproject/scikit-build-core work has since been completed.
- SparseMatrix.hpp currently exposes Eigen publicly; backend-neutral usage has improved, but full encapsulation remains future work.
- EigenCGSolver stores last-solve diagnostics in mutable fields because LinearSolver still returns only Vector.
- Postprocessing sign conventions and principal-stress summary semantics need explicit documentation and tests.

## 15. Common integration errors

| Symptom | Cause / resolution |
| --- | --- |
| No EigenCGSolver in Python | Add binding block to bindings/linalg.cpp, rebuild _core, remove stale _core*.so if necessary, and verify imported core path. |
| LinearSolver.hpp not found | Use feast/solvers/LinearSolver.hpp. |
| Vector(Eigen::VectorXd) fails | Resize feast::Vector and copy entries, matching EigenDirectSolver. |
| CG does not converge | Check SPD conditions, constraints, rigid-body modes, material validity, mesh quality, and preconditioning. |
| Constraints scale badly | Do not zero complete sparse rows/columns using coeffRef; rebuild once from existing nonzeros. |
| Assembly scales badly | Do not reserve current_size + n*n per element; calculate total expected capacity and reserve once. |
| Old extension remains loaded | Exit the Python process and verify feast._core.__file__. |
| Unexpected mesh copies | Confirm Mesh::elements() returns const std::vector<Element>& and add a static_assert. |
| Timing still labelled Solve | Remember Kernel::solveLinearStatic timing may include assembly and constraints; use internal stage timings. |

## 16. Guidance for the next assistant or contributor

- Inspect repository headers and implementations before assuming locations or constructors; this handover records confirmed and intended APIs, but the repository is authoritative.
- Preserve Mesh purity and the region-to-node boundary-condition resolution layer.
- Do not change Kernel merely to simplify Python unless there is a strong architectural reason.
- Prefer adding LinearSolver implementations rather than embedding solver selection in Kernel.
- Keep Eigen-specific code under linalg and expand SparseMatrix abstractions when non-linalg code needs sparse operations.
- Profile before optimising. Separate element calculation, triplet generation, sparse finalisation, constraints, solver setup, iterations, and result construction.
- Optimise containers with full-size reservation and moves, but preserve mixed-element generality.
- Validate numerical outputs against analytical or trusted reference solutions, not plausibility alone.
- When interpreting deformation plots, record geometry, units, load direction, material, mesh size, and visual scale.
- Update this handover whenever public APIs, binding files, header locations, solver choices, timing results, or geometry capabilities change.

## Appendix A. Command reference

```python
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Run C++ tests
ctest --test-dir build --output-on-failure

# Verify Python package and solver bindings
PYTHONPATH=python python3 - <<'PY'
import feast
import feast._core as core
print(feast.__file__)
print(core.__file__)
print([name for name in dir(core) if "Solver" in name])
PY

# Run complexity benchmark
PYTHONPATH=python python3 python/complexity.py

# Find accessor declarations
grep -R "elements()" -n include src
```

## Appendix B. Performance data to retain

| Metric | Record for every benchmark |
| --- | --- |
| Problem size | h, nodes, elements, total DOFs |
| Boundary conditions | resolved Dirichlet and Neumann counts |
| Sparse structure | nonzeros before constraints, after constraints, nonzeros per row |
| Assembly | DOF generation, triplet generation, setFromTriplets/finalisation |
| Constraints | RHS adjustment and matrix rebuild time |
| Solver | type, setup time, iteration time, iterations, estimated error, convergence |
| Postprocessing | time and key result summaries |
| Environment | Release/Debug, compiler, Eigen version, CPU, Python version |

### Appendix C. Canonical cuboid region names

lowx, highx, lowy, highy, lowz, highz

