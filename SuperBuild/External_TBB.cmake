if(MSVC)
    if(${BUILD_LOCAL})
        find_path(
            TBB_SRC
            NAMES
            Makefile
        )
        if(NOT EXISTS ${TBB_SRC}/Makefile)
            message(FATAL_ERROR "Cannot find TBB Source Path")
        endif()
        ExternalProject_Add(
            TBB
            TMP_DIR
            ${CMAKE_BINARY_DIR}/tmp
            STAMP_DIR
            ${CMAKE_BINARY_DIR}/stamp
            DOWNLOAD_DIR
            ${CMAKE_BINARY_DIR}
            SOURCE_DIR
            ${CMAKE_BINARY_DIR}/TBB
            BINARY_DIR
            ${CMAKE_BINARY_DIR}/TBB-build
            INSTALL_DIR
            ${CMAKE_BINARY_DIR}/TBB-install
            DOWNLOAD_COMMAND 
            ${CMAKE_COMMAND} -E copy_directory ${TBB_SRC} ${CMAKE_BINARY_DIR}/TBB
            CONFIGURE_COMMAND
            ""
            BUILD_COMMAND
            ""
            INSTALL_COMMAND
            ""
        )
    else()
        ExternalProject_Add(
            TBB
            TMP_DIR
            ${CMAKE_BINARY_DIR}/tmp
            STAMP_DIR
            ${CMAKE_BINARY_DIR}/stamp
            DOWNLOAD_DIR
            ${CMAKE_BINARY_DIR}
            SOURCE_DIR
            ${CMAKE_BINARY_DIR}/TBB
            BINARY_DIR
            ${CMAKE_BINARY_DIR}/TBB-build
            INSTALL_DIR
            ${CMAKE_BINARY_DIR}/TBB-install
            GIT_REPOSITORY
            "https://github.com/01org/tbb.git"
            GIT_TAG
            "2018_U5"
            CONFIGURE_COMMAND
            ""
            BUILD_COMMAND
            ""
            INSTALL_COMMAND
            ""
        )
    endif()
    ExternalProject_Add_Step(
        TBB
        release_build
        COMMAND
        "${CMAKE_MAKE_PROGRAM}"
        "/m"
        "/p:Configuration=Release"
        "/p:Platform=X64"
        "/p:PlatformToolset=v140"
        "${CMAKE_BINARY_DIR}/TBB/build/vs2013/makefile.sln"
        DEPENDEES
        build
    )
    ExternalProject_Add_Step(
        TBB
        debug_build
        COMMAND
        "${CMAKE_MAKE_PROGRAM}"
        "/m"
        "/p:Configuration=Debug"
        "/p:Platform=X64"
        "/p:PlatformToolset=v140"
        "${CMAKE_BINARY_DIR}/TBB/build/vs2013/makefile.sln"
        DEPENDEES
        build
    )
    ExternalProject_Add_Step(
        TBB
        build_copy
        COMMAND
        ${CMAKE_COMMAND}
        -DTBB:PATH=${CMAKE_BINARY_DIR}/TBB
        -DTBB_BUILD:PATH=${CMAKE_BINARY_DIR}/TBB-build
        -P
        ${CMAKE_CURRENT_SOURCE_DIR}/External_TBBCopy.cmake
        DEPENDEES 
        debug_build
        release_build
    )
    set(TBB_LIBRARY_DEBUG ${CMAKE_BINARY_DIR}/TBB-build/bin/Debug/tbb_debug.lib)
    set(TBB_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/Release/tbb.lib)
    set(TBB_MALLOC_LIBRARY_DEBUG ${CMAKE_BINARY_DIR}/TBB-build/bin/Debug/tbbmalloc_debug.lib)
    set(TBB_MALLOC_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/Release/tbbmalloc.lib)
    set(TBB_MALLOC_PROXY_LIBRARY_DEBUG  ${CMAKE_BINARY_DIR}/TBB-build/bin/Debug/tbbmalloc_proxy_debug.lib)
    set(TBB_MALLOC_PROXY_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/Release/tbbmalloc_proxy.lib)
else()
    if(${BUILD_LOCAL})
        find_path(
            TBB_SRC
            NAMES
            Makefile
        )
        if(NOT EXISTS ${TBB_SRC}/Makefile)
            message(FATAL_ERROR "Cannot find TBB Source Path")
        endif()
        ExternalProject_Add(
            TBB
            TMP_DIR
            ${CMAKE_BINARY_DIR}/tmp
            STAMP_DIR
            ${CMAKE_BINARY_DIR}/stamp
            DOWNLOAD_DIR
            ${CMAKE_BINARY_DIR}
            SOURCE_DIR
            ${CMAKE_BINARY_DIR}/TBB
            BINARY_DIR
            ${CMAKE_BINARY_DIR}/TBB-build
            INSTALL_DIR
            ${CMAKE_BINARY_DIR}/TBB-install
            DOWNLOAD_COMMAND 
            ${CMAKE_COMMAND} -E copy_directory ${TBB_SRC} ${CMAKE_BINARY_DIR}/TBB
            CONFIGURE_COMMAND
            ""
            BUILD_COMMAND
            "${CMAKE_MAKE_PROGRAM}"
            "-j"
            "-C"
            "${CMAKE_BINARY_DIR}/TBB"
            INSTALL_COMMAND
            ""
        )
    else()
        ExternalProject_Add(
            TBB
            TMP_DIR
            ${CMAKE_BINARY_DIR}/tmp
            STAMP_DIR
            ${CMAKE_BINARY_DIR}/stamp
            DOWNLOAD_DIR
            ${CMAKE_BINARY_DIR}
            SOURCE_DIR
            ${CMAKE_BINARY_DIR}/TBB
            BINARY_DIR
            ${CMAKE_BINARY_DIR}/TBB-build
            INSTALL_DIR
            ${CMAKE_BINARY_DIR}/TBB-install
            GIT_REPOSITORY
            "https://github.com/01org/tbb.git"
            GIT_TAG
            "2018_U5"
            CONFIGURE_COMMAND
            ""
            BUILD_COMMAND
            "${CMAKE_MAKE_PROGRAM}"
            "-j"
            "-C"
            "${CMAKE_BINARY_DIR}/TBB"
            INSTALL_COMMAND
            ""
        )
    endif()
    ExternalProject_Add_Step(
        TBB
        build_copy
        COMMAND
        ${CMAKE_COMMAND}
        -DTBB:PATH=${CMAKE_BINARY_DIR}/TBB
        -DTBB_BUILD:PATH=${CMAKE_BINARY_DIR}/TBB-build
        -P
        ${CMAKE_CURRENT_SOURCE_DIR}/External_TBBCopy.cmake
        DEPENDEES 
        build
    )
    set(TBB_LIBRARY_DEBUG ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbb_debug.so)
    set(TBB_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbb.so)
    set(TBB_MALLOC_LIBRARY_DEBUG ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbbmalloc_debug.so)
    set(TBB_MALLOC_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbbmalloc.so)
    set(TBB_MALLOC_PROXY_LIBRARY_DEBUG  ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbbmalloc_proxy_debug.so)
    set(TBB_MALLOC_PROXY_LIBRARY_RELEASE ${CMAKE_BINARY_DIR}/TBB-build/bin/libtbbmalloc_proxy.so)

endif()

add_custom_target(
	TBB_bin
	ALL
	${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/TBB-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	DEPENDS
	TBB
)
set(TBB TBB)
set(VTK_SMP_IMPLEMENTATION_TYPE TBB)
set(TBB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/TBB/include)
set(TBB_MALLOC_INCLUDE_DIR ${CMAKE_BINARY_DIR}/TBB/include)
set(TBB_MALLOC_PROXY_INCLUDE_DIR ${CMAKE_BINARY_DIR}/TBB/include)

