set(v_FindPackageBasedIntegration FALSE)

set(ImportedLibName googletest)
if(v_FindPackageBasedIntegration)
    message(FATAL_ERROR)
    set(ImportedLibNameVersioned ${ImportedLibName}_1_16_Official)
else()
    set(ImportedLibNameVersioned ${ImportedLibName}_1_16_Custom_Partial)
endif()
set(v_ImportedLibRootDirPath ${c_RootThirdPartyDirPath}/${ImportedLibName})
set(v_UnzippedDirPath ${v_ImportedLibRootDirPath}/${ImportedLibNameVersioned})
set(v_ZipFileName ${ImportedLibNameVersioned}.zip)
set(v_SrcZipAddrFilePath ${c_StorageAddrPath}/ThirdParty/${ImportedLibName}/${c_ProjectPlatform}/${v_ZipFileName})
if(WIN32)
    set(v_SrcZipCloudFilePath https://drive.usercontent.google.com/download?id=1kzLGN4DR1a9kCfpXXRryFoCSoIjWkVmw&export=download&authuser=0&confirm=t&uuid=024f3043-530f-4caf-a765-9a7843b50545&at=ALoNOgn1Atiw1tFcNRggeipWpJIm:1748703339322)
elseif(LINUX)
    set(v_SrcZipCloudFilePath https://drive.usercontent.google.com/download?id=1Z_3n4hewCHACvcCgUrtLxUNP2JVEJqTS&export=download&authuser=0&confirm=t&uuid=76f98c78-1831-4efb-aa53-3162aaf879ae&at=ALoNOgnwnfo4Tzi4fyWHDtCEtccM:1748703371157)
else()
	message(FATAL_ERROR "Platform not supported")
endif()
if(v_FindPackageBasedIntegration)
    message(FATAL_ERROR)
    set(v_LibNameFindPackgeBased GTest)
    list(APPEND v_ListComponentFindPackgeBased 
        GTest
        )
    set(GTest_ROOT "${v_UnzippedDirPath}")
else()
    #set(v_DebugLibFileNamePostfix d)
    set(v_LibPlatformArchDirPath ${v_UnzippedDirPath}/build/${c_ProjectPlatform}/${c_ProjectArch})
    list(APPEND v_ListLibIncludeDirPathPrivate ${v_UnzippedDirPath}/include)
    list(APPEND v_ListImportedLibFileName gtest)
endif()

include(${c_RootCMakeProjectFrameworkDirPath}/ImportLibDownloaded.cmake)