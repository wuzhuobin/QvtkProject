# @file		TemplateCMakeLists.txt
# @brief	A template CMakeLists.txt which can automatically generate project
# @author	WUZHUOBIN
# @version	1.6.0
# @since	Sep.19.2017
# @date 	Apr.19.2018
# 
# <h1>How to use</h1>
# <ul>
# 	<li>Create a folder to place all header files, implementation files and the 'TemplateCMakeLists.txt'.</li>
# 	<li>Rename the folder as the project name. Since the folder name will become the project name.</li>
# 	<li>Rename 'TemplateCMakeLists.txt' to 'CMakeLists.txt'.</li>
# 	<li>Configure with CMake and follow normal cmake pipeline.</li>
# 	<li>If build executable, create 'main/main.cpp' as the program entry point.</li>
# 	<li>If build library, no need to do the previous procedure.</li>
# 	<li>DO NOT modify things not in 'CHANGE BELOW'. </li>
# 	<li>If need other Qt module, add modules in 'DEPENDENCY' module.</li>
# 	<li>If need other package, add packages in 'DEPENDENCY' module.</li>
# 	<li>If need Qt qrc, add the path in <code>PROJECT_QRC</code> in 'DEPENDENCY' module. </li>
# 	<li>If need resource, add resources in 'DEPENDENCY' module.</li>
# 	<li>If need other libraries and headers, add them in <code>SYSTEM_LIBRARIES</code> and <code>SYSTEM_INCLUDE</code> in 'LIBRARIES' module. </li>
# </ul>
# <h1>What happen</h1>
# <ul>
# 	<li>CMake will find all '*.c*' '*.h*' files for building. </li>
# 	<li>CMake will find all '*.ui' as Qt UI form. </li>
# 	<li>This template can only build static library. </li>
# 	<li>If 'main/main.cpp' exists, the project will create both static library files and executable files. </li>
# </ul>
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#			Copyright (c) WUZHUOBIN 											    #
#			All rights reserved.												    #
#			See Copyright.txt													    #
#			This software is distributed WITHOUT ANY WARRANTY; without even		    #
#			the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR	    #
#			PURPOSE.  See the above copyright notice for more information.		    #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required(VERSION 3.1) 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
get_filename_component(
	FOLDER_NAME 
	${CMAKE_CURRENT_SOURCE_DIR} 
	NAME
)
project(${FOLDER_NAME})

message(
	STATUS 
	"Building submodule ${PROJECT_NAME}..."
)

# multi-processor build
if(${MSVC})
	set(
		CMAKE_CXX_FLAGS 
		"${CMAKE_CXX_FLAGS} /MP"
	)
	set(
		CMAKE_C_FLAGS
		"${CMAKE_C_FLAGS} /MP"
	)
endif()

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # POLICY  # # # # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# # # # # # # # # # # # # # # # # CHANGE BELOW  # # # # # # # # # # # # # # # # 


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # # DEPENDENCY  # # # # # # # # # # # # # # # # 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# Adding dependency library you need
# Please remember to set it to SYSTEM_LIBRARIES as well 

# QT
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5Core REQUIRED)

# Git
find_package(Git REQUIRED)


# # # # # # # # # # # # # # # # # CHANGE BELOW  # # # # # # # # # # # # # # # # 
option(BUILD_EXAMPLE "Build Example" OFF)
option(BUILD_TESTING "Build Testing" OFF)
option(BUILD_DOCUMENTATION "Build Documentation" OFF)
option (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
option(BUILD_LOCAL "NOT git clone" OFF)
if(NOT ${BUILD_SHARED_LIBS} AND NOT MSVC)
	message(SEND_ERROR "Please check BUILD_SHARED_LIBS. ")
	message(SEND_ERROR "Qt5_ExtrasX11 is needed in STATIC LIBRARY except for MSVC.")
endif()

if(NOT ${BUILD_SHARED_LIBS})
	message(SEND_ERROR "Please check BUILD_SHARED_LIBS. ")
	message(SEND_ERROR "CTK ERROR: INSTALL(EXPORT) given unknown export CTKExports. ")
endif()
include(ExternalProject)
include(External_VTK.cmake)
include(External_ITK.cmake)
include(External_CTK.cmake)
include(External_OpenCV.cmake)
include(External_TBB.cmake)

ExternalProject_Add(
	${CMAKE_PROJECT_NAME}
	DEPENDS
	VTK
	ITK
	CTK
	OpenCV
	TMP_DIR
	${CMAKE_BINARY_DIR}/tmp
	STAMP_DIR
	${CMAKE_BINARY_DIR}/stamp
	DOWNLOAD_DIR
	${CMAKE_BINARY_DIR}
	SOURCE_DIR
	${CMAKE_SOURCE_DIR}
	BINARY_DIR
	${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-build
	INSTALL_DIR
	${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-install
	INSTALL_COMMAND
	""
	CMAKE_CACHE_ARGS
	-DSUPER_BUILD:BOOL=OFF
	-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
	-DBUILD_DOCUMENTATION:BOOL=${BUILD_DOCUMENTATION}
	-DBUILD_EXAMPLE:BOOL=${BUILD_EXAMPLE}
	-DBUILD_TESTING:BOOL=${BUILD_TESTING}
	-DCMAKE_PREFIX_PATH:PATH=${Qt5_DIR}/..;${CMAKE_BINARY_DIR}
)

MESSAGE(STATUS "Building submodule ${PROJECT_NAME} finished!")
