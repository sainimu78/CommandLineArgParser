get_filename_component(c_RootDirPath "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(c_RootCMakeProjectFrameworkDirPath ${c_RootDirPath}/ThirdParty/CMakeProjectFramework/CMakeInclude)
include(${c_RootCMakeProjectFrameworkDirPath}/InitDefaultRootPaths.cmake)