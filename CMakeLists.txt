cmake_minimum_required(VERSION 3.25)
project(CommissionEmulator C)

set(CMAKE_C_STANDARD 17)


string(TOUPPER "${CMAKE_BUILD_TYPE}" build_type)
message("Build type : ${build_type}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type} "${CMAKE_CURRENT_SOURCE_DIR}\\bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type} "${CMAKE_CURRENT_SOURCE_DIR}\\bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type}  "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY_${build_type}  "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\pdb")

add_subdirectory(./src)
