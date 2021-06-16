cmake_minimum_required(VERSION 3.1.0)

project(PangolinShaders)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Debug)

find_package(Pangolin 0.1 REQUIRED)
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

find_path(OPENNI2_INCLUDE_DIR OpenNI.h
          HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_INCLUDEDIR} ${PC_OPENNI_INCLUDE_DIRS} /media/user/Data/DeepLearning/Fusion/deps/OpenNI2/Include /media/user/Data/DeepLearning/Fusion/deps/OpenNI2/Include/ni2
          PATHS "$ENV{PROGRAMFILES}/OpenNI2/Include" "$ENV{PROGRAMW6432}/OpenNI2/Include"
          PATH_SUFFIXES openni ni)
#add a hint so that it can find it without the pkg-config
find_library(OPENNI2_LIBRARY
             NAMES OpenNI2
             HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_LIBDIR} ${PC_OPENNI_LIBRARY_DIRS} /media/user/Data/DeepLearning/Fusion/deps/OpenNI2/Bin/x64-Release
             PATHS "$ENV{PROGRAMFILES}/OpenNI2/Redist" "$ENV{PROGRAMW6432}/OpenNI2/Redist" "$ENV{PROGRAMW6432}/OpenNI2"
             PATH_SUFFIXES lib lib64
)



find_package(CUDA 11.1 REQUIRED)

set(PCL_DIR "/media/user/Data/programs/installed/share/pcl-1.9/")
find_package(PCL 1.9 REQUIRED)

link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${PCL_LIBRARY_DIRS})

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${Pangolin_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${PCL_INCLUDE_DIRS})
include_directories($${OPENNI2_INCLUDE_DIR})

set(CUDA_ARCH_BIN " 52 61" CACHE STRING "Specify 'real' GPU arch     to build binaries for, BIN(PTX) format is supported. Example: 1.3 2.1(1.3) o    r 13 21(13)")
set(CUDA_ARCH_PTX "" CACHE STRING "Specify 'virtual' PTX arch to build PTX intermediate code for. Example: 1.0 1.2 or 10 12")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}  "-Xcompiler;-fPIC; -lineinfo; --source-in-ptx ;")
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "--ftz=true;--prec-div=false;--prec-sqrt=false")




add_executable(PangolinShaders main.cpp ${cuda} ${cuda_objs})
target_link_libraries(PangolinShaders
                         my_python3.7m
                         ${Pangolin_LIBRARIES}
                         ${CUDA_LIBRARIES}
                         ${CUDA_CUDA_LIBRARY}
                         ${PYTHON_LIBRARY}
                         ${OPENNI2_LIBRARY}
                         ${PCL_LIBRARIES}
                      )
