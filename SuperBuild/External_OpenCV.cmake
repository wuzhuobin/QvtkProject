if(${BUILD_LOCAL})

    find_path(
        OpenCV_SRC
        CMakeLists.txt
    )

    if(NOT EXISTS ${OpenCV_SRC}/CMakeLists.txt)
        message(FATAL_ERROR "Cannot find OpenCV Source Path")
    endif()

    ExternalProject_Add(
        OpenCV
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/OpenCV
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/OpenCV-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/OpenCV-install
        DOWNLOAD_COMMAND 
        ${CMAKE_COMMAND} -E copy_directory ${OpenCV_SRC} ${CMAKE_BINARY_DIR}/OpenCV
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
        -DCMAKE_CXX_MP_FLAG:BOOL=ON
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_WITH_STATIC_CRT:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_opencv_java:BOOL=OFF
        -DBUILD_opencv_python3:BOOL=OFF
        -DWITH_MATLAB:BOOL=OFF
        -DWITH_CUDA:BOOL=OFF
    )

else()

    ExternalProject_Add(
        OpenCV
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/OpenCV
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/OpenCV-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/OpenCV-install
        GIT_REPOSITORY
        "https://github.com/opencv/opencv.git"
        GIT_TAG
        "3.3.0"
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
        -DCMAKE_CXX_MP_FLAG:BOOL=ON
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_WITH_STATIC_CRT:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_opencv_java:BOOL=OFF
        -DBUILD_opencv_python3:BOOL=OFF
        -DWITH_MATLAB:BOOL=OFF
        -DWITH_CUDA:BOOL=OFF
    )

endif()

add_custom_target(
	OpenCV_bin
	ALL
	${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/OpenCV-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	DEPENDS
	OpenCV
)
