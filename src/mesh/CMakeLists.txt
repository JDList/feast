add_library(feast_mesh
    Mesh.cpp
)

target_include_directories(feast_mesh
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(feast_mesh PUBLIC cxx_std_20)
