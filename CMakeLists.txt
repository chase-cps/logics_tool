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
    ${CHASEPATH}/chase/include/backends
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/LogicsParser
)

set(Logics_PARSER_SRC_DIR ${PROJECT_SOURCE_DIR}/src/LogicsParser)

set(Logics_PARSER_SRCS
        ${Logics_PARSER_SRC_DIR}/LogicsContractsBaseVisitor.cpp
        ${Logics_PARSER_SRC_DIR}/LogicsContractsLexer.cpp
        ${Logics_PARSER_SRC_DIR}/LogicsContractsVisitor.cpp
        ${Logics_PARSER_SRC_DIR}/LogicsContractsParser.cpp)

set(Logics_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(Logics_Library_SRCS
        ${Logics_SRC_DIR}/Console.cc
        ${Logics_SRC_DIR}/Console_commands.cc
        ${Logics_PARSER_SRCS}
        ${Logics_SRC_DIR}/LogicsSpecsBuilder.cc
)

set(Logics_tool_SRCS
    ${Logics_Library_SRCS}
    ${Logics_SRC_DIR}/main.cc)


if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(chase_logicsLang
    ${Logics_Library_SRCS}
    )

install(TARGETS chase_logicsLang
    DESTINATION chase/lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION chase/include/frontends
    )

add_executable(logics_tool 
    ${Logics_tool_SRCS})

target_link_libraries(logics_tool 
    chase
    chase-backends
    antlr4-runtime)

target_link_directories(
    logics_tool
    PRIVATE ${CHASEPATH}/chase/lib
    PRIVATE ${CHASEPATH}/third_party/antlr4/lib)

install(TARGETS logics_tool DESTINATION chase/bin)

pybind11_add_module(
    pychase_logicsLang MODULE
    src/bindings.cc
    ${Logics_Library_SRCS}
    )

target_link_libraries(pychase_logicsLang PRIVATE
    chase-backends
    antlr4-runtime
    chase)

target_link_directories(
    pychase_logicsLang
    PRIVATE ${CHASEPATH}/chase/lib
    PRIVATE ${CHASEPATH}/third_party/antlr4/lib
    )
 
install(TARGETS pychase_logicsLang
    EXPORT pychaseTargets
    DESTINATION chase/lib)
