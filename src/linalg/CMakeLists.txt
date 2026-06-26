add_library(feast_linalg
    Vector.cpp
    DenseMatrix.cpp
    SparseMatrix.cpp
)

target_include_directories(feast_linalg
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/eigen
)

target_compile_features(feast_linalg PUBLIC cxx_std_20)
