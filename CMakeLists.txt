#Based on
#https://github.com/antlr/antlr4/tree/master/runtime/Cpp/cmake

CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)
project(rtfss_compiler)

find_package(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer") #TODO remember to remove
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
#set(ANTLR4_ZIP_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/antlr4-master.zip)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/antlr4.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(rtfssGrammar src/lexer_parser/rtfss.g4 VISITOR)

# include generated files in project environment
#message(STATUS "${ANTLR_rtfssGrammar_OUTPUT_DIR}")
include_directories(${ANTLR_rtfssGrammar_OUTPUT_DIR})

aux_source_directory(src/ast RTFSS_AST_SOURCE)
add_library(RTFSS_AST STATIC ${RTFSS_AST_SOURCE})

aux_source_directory(src/lexer_parser RTFSS_LEXER_PARSER_SOURCE)
add_library(RTFSS_LEXER_PARSER STATIC ${RTFSS_LEXER_PARSER_SOURCE})

aux_source_directory(src/const_solver RTFSS_CONST_SOLVER_SOURCE)
add_library(RTFSS_CONST_SOLVER STATIC ${RTFSS_CONST_SOLVER_SOURCE})

aux_source_directory(src/var_solver RTFSS_VAR_SOLVER_SOURCE)
add_library(RTFSS_VAR_SOLVER STATIC ${RTFSS_VAR_SOLVER_SOURCE})

aux_source_directory(src/assign_handler RTFSS_ASSIGN_HANDLER_SOURCE)
add_library(RTFSS_ASSIGN_HANDLER STATIC ${RTFSS_ASSIGN_HANDLER_SOURCE})

aux_source_directory(src/arch_gen RTFSS_ARCH_GEN_SOURCE)
add_library(RTFSS_ARCH_GEN STATIC ${RTFSS_ARCH_GEN_SOURCE})

aux_source_directory(src/frepr_gen RTFSS_FREPR_GEN_SOURCE)
add_library(RTFSS_FREPR_GEN STATIC ${RTFSS_FREPR_GEN_SOURCE})

aux_source_directory(src/target_gen/vhdl RTFSS_TARGET_GEN_VHDL_SOURCE)
add_library(RTFSS_TARGET_GEN_VHDL STATIC ${RTFSS_TARGET_GEN_VHDL_SOURCE})

aux_source_directory(src/error_handler RTFSS_ERROR_HANDLER_SOURCE)
add_library(RTFSS_ERROR_HANDLER STATIC ${RTFSS_ERROR_HANDLER_SOURCE})

add_library(RTFSS_ANTLR4 STATIC ${ANTLR_rtfssGrammar_CXX_OUTPUTS})

add_executable(rtfss src/rtfss.cpp)

target_link_libraries(rtfss
						RTFSS_AST
						RTFSS_LEXER_PARSER
						RTFSS_CONST_SOLVER
						RTFSS_VAR_SOLVER
						RTFSS_ASSIGN_HANDLER
						RTFSS_ARCH_GEN
						RTFSS_FREPR_GEN
						RTFSS_TARGET_GEN_VHDL
						RTFSS_ERROR_HANDLER)

target_link_libraries(  RTFSS_ANTLR4
						antlr4_static)

target_link_libraries(RTFSS_LEXER_PARSER
						RTFSS_AST
						RTFSS_ANTLR4
						RTFSS_ERROR_HANDLER
						antlr4_static)

target_link_libraries(RTFSS_CONST_SOLVER
						RTFSS_ERROR_HANDLER
						RTFSS_AST)

target_link_libraries(RTFSS_VAR_SOLVER
						RTFSS_ERROR_HANDLER
						RTFSS_AST)

target_link_libraries(RTFSS_ASSIGN_HANDLER
						RTFSS_ERROR_HANDLER
						RTFSS_AST)

target_link_libraries(RTFSS_ARCH_GEN
						RTFSS_AST
						RTFSS_VAR_SOLVER
						RTFSS_ERROR_HANDLER
						${Boost_LIBRARIES})

target_link_libraries(RTFSS_FREPR_GEN
						RTFSS_AST
						RTFSS_VAR_SOLVER
						RTFSS_ARCH_GEN
						RTFSS_ERROR_HANDLER
						${Boost_LIBRARIES})

target_link_libraries(RTFSS_TARGET_GEN_VHDL
						RTFSS_AST
						RTFSS_VAR_SOLVER
						RTFSS_ARCH_GEN
						RTFSS_FREPR_GEN
						RTFSS_ERROR_HANDLER
						${Boost_LIBRARIES})

add_custom_target(compdatabase ALL
				  COMMAND cp ${PROJECT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
				  DEPENDS ${PROJECT_BINARY_DIR}/compile_commands.json)
