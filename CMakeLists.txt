###############################################################################
# CMake file for the CHASE tool managing Logics-based specifications.
# @author Michele Lora
###############################################################################

cmake_minimum_required (VERSION 3.10)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS_DEBUG "-O0 -Wall -Wextra -pedantic -ggdb -fPIC")
set(CMAKE_C_FLAGS_RELEASE "-O3 -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -Wextra -pedantic -ggdb -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3  -fPIC")


find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)

project(LOGICS_TOOL VERSION 1.0)

set(CHASEPATH ../../install_dir CACHE PATH "The path of the Chase installation")

include_directories(
    ${CHASEPATH}/third_party/antlr4/include
    ${CHASEPATH}/chase/include
    ${CHASEPATH}/chase/include/utilities
    ${CHASEPATH}/chase/include/rerpesentation
    ${CHASEPATH}/chase/include/frontends
    ${CHASEPATH}/chase/include/frontends/LogicsParser
    ${CHASEPATH}/chase/include/backends
    ${PROJECT_SOURCE_DIR}/include
)

set(Logics_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(Logics_TOOL_SRCS
        ${Logics_SRC_DIR}/main.cc
        ${Logics_SRC_DIR}/Console.cc
        ${Logics_SRC_DIR}/Console_commands.cc
)


if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(logics_tool ${Logics_TOOL_SRCS})
target_link_libraries(logics_tool 
    chase
    chase_ll
    chase_backends
    antlr4-runtime)

target_link_directories(
    logics_tool
    PRIVATE ${CHASEPATH}/chase/lib
    PRIVATE ${CHASEPATH}/third_party/antlr4/lib)

install(TARGETS logics_tool DESTINATION chase/bin)

pybind11_add_module(
    pychase_logics_console MODULE
    src/bindings.cc
    src/Console.cc
    src/Console_commands.cc)

target_link_libraries(pychase_logics_console PRIVATE
    chase
    chase_backends)

target_link_directories(
    pychase_logics_console
    PRIVATE ${CHASEPATH}/chase/lib
    )
 
install(TARGETS pychase_logics_console
    EXPORT pychaseTargets
    DESTINATION chase/lib)
