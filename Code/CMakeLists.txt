cmake_minimum_required(VERSION 3.10)
project (Taskification)

find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_llvm_library(TaskFunctions MODULE Taskification.cc PLUGIN_TOOL clang)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  target_link_libraries(TaskFunctions PRIVATE
    clangAST
    clangBasic
    clangFrontend
    LLVMSupport
    )
endif() 
add_subdirectory(Taskification)


