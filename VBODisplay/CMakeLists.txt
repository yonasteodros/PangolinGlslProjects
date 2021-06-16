cmake_minimum_required(VERSION 3.1.0)

project(PangolinCuda)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Debug)

find_package(Pangolin 0.1 REQUIRED)
# This example could be made to work with C++11, but the kernel code must be
# compiled without it.
#if(CUDA_FOUND)
#  cuda_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
#  cuda_add_executable(
#    VBODisplay
#    main.cpp kernal.cu
#  )

#endif()

add_library(my_python3.7m SHARED IMPORTED)
set_target_properties(my_python3.7m PROPERTIES
        IMPORTED_LOCATION "/home/user/anaconda3/envs/my_env/lib/libpython3.7m.so.1.0"
        INTERFACE_INCLUDE_DIRECTORIES "/home/user/anaconda3/envs/my_env/include/python3.7m/"
        )
set(PYTHON_INCLUDE_DIR "/home/user/anaconda3/envs/my_env/include/python3.7m/")
set(PYTHON_LIBRARY "/home/user/anaconda3/envs/my_env/lib/libpython3.7m.so.1.0")
message("Using: ")
message("=======")
message("Using python:  ${PYTHONLIBS_VERSION_STRING} ")
message("Using PYTHON_LIBRARIES:  ${PYTHON_LIBRARY} ")
message("Using PYTHON_INCLUDE_DIRS:  ${PYTHON_INCLUDE_DIR} ")
message("=======")

find_package(CUDA 11.1 REQUIRED)

link_directories(${CUDA_LIBRARY_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${Pangolin_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB cuda cuda/*.cu)
set(CUDA_ARCH_BIN " 52 61" CACHE STRING "Specify 'real' GPU arch     to build binaries for, BIN(PTX) format is supported. Example: 1.3 2.1(1.3) o    r 13 21(13)")
set(CUDA_ARCH_PTX "" CACHE STRING "Specify 'virtual' PTX arch to build PTX intermediate code for. Example: 1.0 1.2 or 10 12")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}  "-Xcompiler;-fPIC; -lineinfo; --source-in-ptx ;")
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "--ftz=true;--prec-div=false;--prec-sqrt=false")
CUDA_COMPILE(cuda_objs ${cuda})

add_executable(PangolinCuda main.cpp ${cuda} ${cuda_objs})
target_link_libraries(PangolinCuda
                         my_python3.7m
                         ${Pangolin_LIBRARIES}
                         ${CUDA_LIBRARIES}
                         ${CUDA_CUDA_LIBRARY}
                         ${PYTHON_LIBRARY}
                      )
