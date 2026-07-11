include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6
)

FetchContent_MakeAvailable(pybind11)

pybind11_add_module(_core
    feast.cpp
    linalg.cpp
    mesh.cpp
    geometry.cpp
    meshing.cpp
    bcs.cpp
    materials.cpp
    elements.cpp
    kernel.cpp
    postprocessing.cpp
    dof.cpp
)

target_link_libraries(_core PRIVATE feast)

target_include_directories(_core PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(_core PROPERTIES
    OUTPUT_NAME "_core"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/feast
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/feast
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bindings
)
