cmake_minimum_required(VERSION 3.13)
project(cs487game)

if (APPLE)
#    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
#    set(CMAKE_C_COMPILER   "/usr/local/opt/llvm/bin/clang")
#    set(CMAKE_AR           "/usr/local/top/llvm/bin/llvm-ar")
#    set(CMAKE_LINKER       "/usr/local/top/llvm/bin/lld")
#    set(CMAKE_NM           "/usr/local/top/llvm/bin/llvm-nm")
#    set(CMAKE_OBJDUMP      "/usr/local/top/llvm/bin/llvm-objdump")
#    set(CMAKE_RANLIB       "/usr/local/top/llvm/bin/llvm-ranlib")
#    include_directories(/usr/local/opt/llvm/include)
#    link_directories(/usr/local/opt/llvm/lib)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
#    set(STD_CXX_FS c++fs)
endif()

include_directories(~/github/box2d/include)
link_directories(~/github/box2d/build/src)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
set(GCC_EXPECTED_VERSION 9.0.0)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_EXPECTED_VERSION)
    message("Using GCC 9 will have the best results")
endif()
endif()

add_subdirectory(gamelib)
add_subdirectory(simplegame)

