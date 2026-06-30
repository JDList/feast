# Auto-generated CMakeLists.txt

add_executable(test_assembly test_assembly.cpp)
target_link_libraries(test_assembly PRIVATE feast)
add_test(NAME test_assembly COMMAND test_assembly)

add_executable(test_bcs test_bcs.cpp)
target_link_libraries(test_bcs PRIVATE feast)
add_test(NAME test_bcs COMMAND test_bcs)

add_executable(test_boundary_condition_resolver test_boundary_condition_resolver.cpp)
target_link_libraries(test_boundary_condition_resolver PRIVATE feast)
add_test(NAME test_boundary_condition_resolver COMMAND test_boundary_condition_resolver)

add_executable(test_constraint_applier test_constraint_applier.cpp)
target_link_libraries(test_constraint_applier PRIVATE feast)
add_test(NAME test_constraint_applier COMMAND test_constraint_applier)

add_executable(test_cuboid_builder test_cuboid_builder.cpp)
target_link_libraries(test_cuboid_builder PRIVATE feast)
add_test(NAME test_cuboid_builder COMMAND test_cuboid_builder)

add_executable(test_dense_matrix test_dense_matrix.cpp)
target_link_libraries(test_dense_matrix PRIVATE feast)
add_test(NAME test_dense_matrix COMMAND test_dense_matrix)

add_executable(test_dof test_dof.cpp)
target_link_libraries(test_dof PRIVATE feast)
add_test(NAME test_dof COMMAND test_dof)

add_executable(test_geometry test_geometry.cpp)
target_link_libraries(test_geometry PRIVATE feast)
add_test(NAME test_geometry COMMAND test_geometry)

add_executable(test_kernel test_kernel.cpp)
target_link_libraries(test_kernel PRIVATE feast)
add_test(NAME test_kernel COMMAND test_kernel)

add_executable(test_materials test_materials.cpp)
target_link_libraries(test_materials PRIVATE feast)
add_test(NAME test_materials COMMAND test_materials)

add_executable(test_mesh test_mesh.cpp)
target_link_libraries(test_mesh PRIVATE feast)
add_test(NAME test_mesh COMMAND test_mesh)

add_executable(test_mesher test_mesher.cpp)
target_link_libraries(test_mesher PRIVATE feast)
add_test(NAME test_mesher COMMAND test_mesher)

add_executable(test_region_bcs test_region_bcs.cpp)
target_link_libraries(test_region_bcs PRIVATE feast)
add_test(NAME test_region_bcs COMMAND test_region_bcs)

add_executable(test_solver test_solver.cpp)
target_link_libraries(test_solver PRIVATE feast)
add_test(NAME test_solver COMMAND test_solver)

add_executable(test_sparse_matrix test_sparse_matrix.cpp)
target_link_libraries(test_sparse_matrix PRIVATE feast)
add_test(NAME test_sparse_matrix COMMAND test_sparse_matrix)

add_executable(test_tet4 test_tet4.cpp)
target_link_libraries(test_tet4 PRIVATE feast)
add_test(NAME test_tet4 COMMAND test_tet4)

add_executable(test_vector test_vector.cpp)
target_link_libraries(test_vector PRIVATE feast)
add_test(NAME test_vector COMMAND test_vector)

