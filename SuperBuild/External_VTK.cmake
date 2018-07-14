if(${BUILD_LOCAL})

	find_path(
		VTK_SRC
		NAMES
		CMakeLists.txt
	)

	if(NOT EXISTS ${VTK_SRC}/CMakeLists.txt)
		message(FATAL_ERROR "Cannot find VTK Source Path")
	endif()

	ExternalProject_Add(
		VTK
		TMP_DIR
		${CMAKE_BINARY_DIR}/tmp
		STAMP_DIR
		${CMAKE_BINARY_DIR}/stamp
		DOWNLOAD_DIR
		${CMAKE_BINARY_DIR}
		SOURCE_DIR
		${CMAKE_BINARY_DIR}/VTK
		BINARY_DIR
		${CMAKE_BINARY_DIR}/VTK-build
		INSTALL_DIR
		${CMAKE_BINARY_DIR}/VTK-install
		DOWNLOAD_COMMAND 
		${CMAKE_COMMAND} -E copy_directory ${VTK_SRC} ${CMAKE_BINARY_DIR}/VTK
		INSTALL_COMMAND
		""
		CMAKE_CACHE_ARGS
		-DCMAKE_CXX_MP_FLAG:BOOL=ON
		-DBUILD_DOCUMENTATION:BOOL=OFF
		-DBUILD_EXAMPLES:BOOL=OFF
		-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
		-DBUILD_TESTING:BOOL=OFF
		-DModule_vtkImagingOpenGL2:BOOL=ON
		-DModule_vtkIOExportOpenGL2:BOOL=ON
		-DModule_vtkRenderingLICOpenGL2:BOOL=ON
		-DVTK_QT_VERSION:STRING=5
		-DVTK_Group_Qt:BOOL=ON
		-DModule_vtkGUISupportQt:BOOL=ON
		-DModule_vtkGUISupportQtOpenGL:BOOL=ON
		-DModule_vtkGUISupportQtSQL:BOOL=ON
		-DModule_vtkGUISupportQtWebkit:BOOL=OFF
		-DQt5_DIR:PATH=${Qt5_DIR}
		-DQt5Core_DIR:PATH=${Qt5_DIR}Core
		-DQt5Gui_DIR:PATH=${Qt5_DIR}Gui
		-DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
		-DQt5Sql_DIR:PATH=${Qt5_DIR}Sql
		-DQt5UiPlugin_DIR:PATH=${Qt5_DIR}UiPlugin
		-DQt5Widgets_DIR:PATH=${Qt5_DIR}Widgets
    )

else()

    ExternalProject_Add(
        VTK
        TMP_DIR
        ${CMAKE_BINARY_DIR}/tmp
        STAMP_DIR
        ${CMAKE_BINARY_DIR}/stamp
        DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}
        SOURCE_DIR
        ${CMAKE_BINARY_DIR}/VTK
        BINARY_DIR
        ${CMAKE_BINARY_DIR}/VTK-build
        INSTALL_DIR
        ${CMAKE_BINARY_DIR}/VTK-install
        GIT_REPOSITORY
        "https://github.com/Kitware/VTK.git"
        GIT_TAG
        "v8.1.1"
        INSTALL_COMMAND
        ""
        CMAKE_CACHE_ARGS
        -DCMAKE_CXX_MP_FLAG:BOOL=ON
        -DBUILD_DOCUMENTATION:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING:BOOL=OFF
        -DModule_vtkImagingOpenGL2:BOOL=ON
        -DModule_vtkIOExportOpenGL2:BOOL=ON
        -DModule_vtkRenderingLICOpenGL2:BOOL=ON
        -DVTK_QT_VERSION:STRING=5
        -DVTK_Group_Qt:BOOL=ON
        -DModule_vtkGUISupportQt:BOOL=ON
        -DModule_vtkGUISupportQtOpenGL:BOOL=ON
        -DModule_vtkGUISupportQtSQL:BOOL=ON
        -DModule_vtkGUISupportQtWebkit:BOOL=OFF
        -DQt5_DIR:PATH=${Qt5_DIR}
        -DQt5Core_DIR:PATH=${Qt5_DIR}Core
        -DQt5Gui_DIR:PATH=${Qt5_DIR}Gui
        -DQt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL
        -DQt5Sql_DIR:PATH=${Qt5_DIR}Sql
        -DQt5UiPlugin_DIR:PATH=${Qt5_DIR}UiPlugin
		-DQt5Widgets_DIR:PATH=${Qt5_DIR}Widgets
		# -DVTK_SMP_IMPLEMENTATION_TYPE:STRING=TBB
		# -DTBB_INCLUDE_DIR:PATH=${CMAKE_BINARY_DIR}/TBB/include
		# -DTBB_LIBRARY_DEBUG:PATH=${CMAKE_BINARY_DIR}/TBB/
    )

endif()

add_custom_target(
	VTK_bin
	ALL
	${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/VTK-build/bin ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build/bin
	DEPENDS
	VTK
)