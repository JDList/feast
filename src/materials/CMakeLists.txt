add_library(feast_materials
    LinearElastic.cpp
)

target_include_directories(feast_materials
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(feast_materials PUBLIC cxx_std_20)
