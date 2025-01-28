cmake_minimum_required(VERSION 3.10.0)

include(../InitRootPath.cmake)

set(c_ProjectName CommandLineArgParser)

include(${c_RootCMakeProjectFrameworkDirPath}/ProjectBegin.cmake)
project(${c_ProjectName})
include(${c_RootCMakeProjectFrameworkDirPath}/ProjectEnd.cmake)

list(APPEND v_ListModuleInstallingHeaderDirPath ${c_RootDirPath}/include)
include(${c_ProjectDirPath}/Install.cmake)

include(${c_RootCMakeProjectFrameworkDirPath}/Release.cmake)
include(${c_RootCMakeProjectFrameworkDirPath}/Cleanup.cmake)