add_executable(test_vector test_vector.cpp)
target_link_libraries(test_vector PRIVATE feast_linalg)
target_include_directories(test_vector PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_dense_matrix test_dense_matrix.cpp)
target_link_libraries(test_dense_matrix PRIVATE feast_linalg)
target_include_directories(test_dense_matrix PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_sparse_matrix test_sparse_matrix.cpp)
target_link_libraries(test_sparse_matrix PRIVATE feast_linalg)
target_include_directories(test_sparse_matrix PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_mesh test_mesh.cpp)
target_link_libraries(test_mesh PRIVATE feast_mesh)
target_include_directories(test_mesh PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_materials test_materials.cpp)
target_link_libraries(test_materials PRIVATE feast_materials)
target_include_directories(test_materials PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_tet4 test_tet4.cpp)
target_link_libraries(test_tet4 PRIVATE feast_elements)
target_include_directories(test_tet4 PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME vector COMMAND test_vector)
add_test(NAME dense_matrix COMMAND test_dense_matrix)
add_test(NAME sparse_matrix COMMAND test_sparse_matrix)
add_test(NAME mesh COMMAND test_mesh)
add_test(NAME materials COMMAND test_materials)
add_test(NAME tet4 COMMAND test_tet4)
