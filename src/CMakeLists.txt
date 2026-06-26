add_library(feast)

target_include_directories(feast
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(feast
    PUBLIC
        Eigen3::Eigen
)

target_compile_features(feast PUBLIC cxx_std_20)

add_subdirectory(linalg)
add_subdirectory(mesh)
add_subdirectory(materials)
add_subdirectory(elements)
add_subdirectory(dof)
add_subdirectory(bcs)
add_subdirectory(assembly)
add_subdirectory(kernel)
