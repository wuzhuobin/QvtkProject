if(${BUILD_LOCAL})

	find_path(
		CTK_SRC
		NAMES
		CMakeLists.txt
	)

	if(NOT EXISTS ${CTK_SRC}/CMakeLists.txt)
		message(FATAL_ERROR "Cannot find CTK Source Path")
	endif()

	ExternalProject_Add(
		CTK
		TMP_DIR
		${CMAKE_BINARY_DIR}/tmp
		STAMP_DIR
		${CMAKE_BINARY_DIR}/stamp
		DOWNLOAD_DIR
		${CMAKE_BINARY_DIR}
		SOURCE_DIR
		${CMAKE_BINARY_DIR}/CTK
		BINARY_DIR
		${CMAKE_BINARY_DIR}/CTK-build
		INSTALL_DIR
		${CMAKE_BINARY_DIR}/CTK-install
		DOWNLOAD_COMMAND 
		${CMAKE_COMMAND} -E copy_directory ${CTK_SRC} ${CMAKE_BINARY_DIR}/CTK
		INSTALL_COMMAND
		""
        CMAKE_CACHE_ARGS
		-DCMAKE_CXX_MP_FLAG:BOOL=ON
		-DBUILD_DOCUMENTATION:BOOL=OFF
		-DCTK_BUILD_EXAMPLES:BOOL=OFF
		-DCTK_BUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING:BOOL=OFF
        # -DCTK_APPctkDICOM:BOOL=ON
        # -DCTK_APPctkDICOM2:BOOL=ON
        -DCTK_ENABLE_DICOM:BOOL=ON
        -DCTK_LIB_DICOM/Core:BOOL=ON
        -DCTK_LIB_DICOM/Widgets:BOOL=ON
        -DCTK_QT_VERSION:STRING=5
        -DQt5_DIR:PATH=${Qt5_DIR}
        # -DQt5Concurrent:PATH=${Qt5_DIR}Concurrent
		# -DQt5Core_DIR:PATH=${Qt5_DIR}Core
		# -DQt5Network_DIR:PATH=${Qt5_DIR}Network
		# -DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
        # -DQt5Sql_DIR:PATH=${Qt5_DIR}Sql
        # -DQt5Test_DIR:PATH=${Qt5_DIR}Test
		# -DQt5Gui_DIR:PATH=${Qt5_DIR}Gui
		# -DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
		# -DQt5UiPlugin_DIR:PATH=${Qt5_DIR}UiPlugin
		# -DQt5Widgets_DIR:PATH=${Qt5_DIR}Widgets
    )

else()

    ExternalProject_Add(
        CTK
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/CTK
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/CTK-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/CTK-install
        GIT_REPOSITORY
        "https://github.com/commontk/CTK.git"
        GIT_TAG
        "524fd8729bbee740392739d22f64784ec81a9804"
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
		-DCMAKE_CXX_MP_FLAG:BOOL=ON
		-DBUILD_DOCUMENTATION:BOOL=OFF
		-DCTK_BUILD_EXAMPLES:BOOL=OFF
		-DCTK_BUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING:BOOL=OFF
        # -DCTK_APPctkDICOM:BOOL=ON
        # -DCTK_APPctkDICOM2:BOOL=ON
        -DCTK_ENABLE_DICOM:BOOL=ON
        -DCTK_LIB_DICOM/Core:BOOL=ON
        -DCTK_LIB_DICOM/Widgets:BOOL=ON
        -DCTK_QT_VERSION:STRING=5
        -DQt5_DIR:PATH=${Qt5_DIR}
        # -DQt5Concurrent:PATH=${Qt5_DIR}Concurrent
		# -DQt5Core_DIR:PATH=${Qt5_DIR}Core
		# -DQt5Network_DIR:PATH=${Qt5_DIR}Network
		# -DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
        # -DQt5Sql_DIR:PATH=${Qt5_DIR}Sql
        # -DQt5Test_DIR:PATH=${Qt5_DIR}Test
		# -DQt5Gui_DIR:PATH=${Qt5_DIR}Gui
		# -DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
		# -DQt5UiPlugin_DIR:PATH=${Qt5_DIR}UiPlugin
		# -DQt5Widgets_DIR:PATH=${Qt5_DIR}Widgets
    )

endif()

add_custom_target(
	CTK_bin
	ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/CTK-build/CTK-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/CTK-build/DCMTK-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	DEPENDS
	CTK
)