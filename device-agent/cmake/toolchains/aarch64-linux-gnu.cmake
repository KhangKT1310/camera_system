set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify target cross compilers
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Where is the target environment root path
if(DEFINED ENV{SYSROOT})
    set(CMAKE_FIND_ROOT_PATH $ENV{SYSROOT})
endif()

# Search programs on the host system
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries only in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
