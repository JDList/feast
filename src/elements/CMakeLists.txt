add_library(feast_elements
    Tet4.cpp
)

target_include_directories(feast_elements
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(feast_elements
    PUBLIC
        feast_linalg
        feast_mesh
        feast_materials
)

target_compile_features(feast_elements PUBLIC cxx_std_20)
