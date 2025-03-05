cmake_minimum_required(VERSION 3.20)

include(../InitRootPath.cmake)

set(c_ProjectName Test)

include(${c_RootCMakeProjectFrameworkDirPath}/ProjectBegin.cmake)
project(${c_ProjectName})
include(${c_RootCMakeProjectFrameworkDirPath}/ProjectEnd.cmake)

enable_testing() 

set(ModuleName TestConsole)
set(ModuleIncludePath ${c_RootDirPath}/include)
set(ModuleSourcePath ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SrcAll ${ModuleSourcePath}/*.cpp ${ModuleIncludePath}/*.h)
add_executable(${ModuleName} ${SrcAll})
target_include_directories(${ModuleName} PRIVATE ${ModuleIncludePath})
include(Licensed_googletest.cmake)
add_test(NAME ${ModuleName} COMMAND ${ModuleName}) # 支持 ctest

set(DefaultModuleName ${ModuleName})

include(${c_RootCMakeProjectFrameworkDirPath}/Cleanup.cmake)

if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${DefaultModuleName})
endif()