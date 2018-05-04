if(${BUILD_LOCAL})

    find_path(
        ITK_SRC
        CMakeLists.txt
    )

    if(NOT EXISTS ${ITK_SRC}/CMakeLists.txt)
        message(FATAL_ERROR "Cannot find ITK Source Path")
    endif()

    ExternalProject_Add(
        ITK
        DEPENDS
        VTK
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/ITK
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/ITK-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/ITK-install
        DOWNLOAD_COMMAND 
        ${CMAKE_COMMAND} -E copy_directory ${ITK_SRC} ${CMAKE_BINARY_DIR}/ITK
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
        -DCMAKE_CXX_MP_FLAG:BOOL=ON
        -DBUILD_DOCUMENTATION:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING:BOOL=OFF
        -DModule_ITKVtkGlue:BOOL=ON
        -DVTK_DIR:PATH=${CMAKE_BINARY_DIR}/VTK-build
        -DModule_ITKReview:BOOL=ON
    )

else()

    ExternalProject_Add(
        ITK
        DEPENDS
        VTK
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/ITK
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/ITK-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/ITK-install
        GIT_REPOSITORY
        "https://github.com/InsightSoftwareConsortium/ITK.git"
        GIT_TAG
        "v4.12.2"
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
        -DCMAKE_CXX_MP_FLAG:BOOL=ON
        -DBUILD_DOCUMENTATION:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING:BOOL=OFF
        -DModule_ITKVtkGlue:BOOL=ON
        -DVTK_DIR:PATH=${CMAKE_BINARY_DIR}/VTK-build
        -DModule_ITKReview:BOOL=ON
    )

endif()

add_custom_target(
	ITK_bin
	ALL
	${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/ITK-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	DEPENDS
	ITK
)
