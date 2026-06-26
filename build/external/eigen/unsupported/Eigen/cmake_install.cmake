# Install script for directory: /home/jdlist/feast/external/eigen/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/AdolcForward"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/AlignedVector3"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/ArpackSupport"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/AutoDiff"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/BVH"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/EulerAngles"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/FFT"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/GPU"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/IterativeSolvers"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/KroneckerProduct"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/LevenbergMarquardt"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/MatrixFunctions"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/MPRealSupport"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/NNLS"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/NonLinearOptimization"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/NumericalDiff"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/OpenGLSupport"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/Polynomials"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/SparseExtra"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/SpecialFunctions"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/Splines"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/Tensor"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/TensorSymmetry"
    "/home/jdlist/feast/external/eigen/unsupported/Eigen/ThreadPool"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/home/jdlist/feast/external/eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/jdlist/feast/build/external/eigen/unsupported/Eigen/CXX11/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/jdlist/feast/build/external/eigen/unsupported/Eigen/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
