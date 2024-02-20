# usage:  cmake -G "MinGW Makefiles" . -Bbuild
#         cmake --build ./build
cmake_minimum_required (VERSION 3.5)
project (crc2hex)

enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(EXECUTABLE ${PROJECT_NAME})

add_definitions("-DJOB_BASE_NAME=\"$ENV{JOB_BASE_NAME}\"")
add_definitions("-DBUILD_NUMBER=\"$ENV{BUILD_NUMBER}\"")
add_definitions("-DGIT_COMMIT=\"$ENV{GIT_COMMIT}\"")
add_definitions("-DGIT_BRANCH=\"$ENV{GIT_BRANCH}\"")

# include directory with headers
include_directories(src)

# add all files from directory to sources
file(GLOB_RECURSE sources	src/*.cpp src/*.hpp)

# set output directory for generated binary file
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
    
# add some more flags for compiler
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra -O2 -s")

# generate binary file from the list of sources
add_executable (${EXECUTABLE} ${sources})