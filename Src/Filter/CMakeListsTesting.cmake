# @file		CMakeListsTesting.cmake
# @brief	A template CMakeListsTesting.cmake which can automatically generate testing.
# @author	WUZHUOBIN
# @version	1.7.0
# @since	Sep.19.2017
# @date 	May.10.2018
# 
# <h1>How to use</h1>
# <ul>
# 	<li>Create a folder  "Testing" to place all testing files.</li>
# 	<li>include this file in the 'CMakeLists.txt'.</li>
# 	<li>Configure with CMake and follow normal cmake pipeline.</li>
# 	<li>DO NOT modify things not in 'CHANGE BELOW'. </li>
# 	<li>If need other Qt module, add modules in 'DEPENDENCY' module.</li>
# 	<li>If need other package, add packages in 'DEPENDENCY' module.</li>
# 	<li>If need other libraries and headers, add them in <code>SYSTEM_LIBRARIES</code> and <code>SYSTEM_INCLUDE</code> in 'LIBRARIES' module. </li>
# </ul>
# <h1>What happen</h1>
# <ul>
# 	<li>CMake will find all '*.c*'. </li>
# 	<li>This template can only build static library. </li>
# 	<li>This CMakeLists is for project with Qt5::test dependency only.</li>
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
message(
	STATUS 
	"Building submodule Testing${PROJECT_NAME}..."
)

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
find_package(Qt5Test REQUIRED)
include(CTest)
# # # # # # # # # # # # # # # # # CHANGE BELOW  # # # # # # # # # # # # # # # # 


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  
# # # # # # # # # # # # # # # # # LIBRARIES # # # # # # # # # # # # # # # # # #  
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  
set(
	CMAKE_RUNTIME_OUTPUT_DIRECTORY 
	${CMAKE_BINARY_DIR}/bin
)
link_directories(
	${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
set(
	SYSTEM_LIBRARIES
	Qt5::Test
)
set(
	SYSTEM_INCLUDE
	${PROJECT_SOURCE_DIR}
	${PROJECT_BINARY_DIR}
)
# # # # # # # # # # # # # # # # # CHANGE BELOW  # # # # # # # # # # # # # # # # 
set(
	SYSTEM_LIBRARIES
	${SYSTEM_LIBRARIES}
	#OTHERS
	#...
	Filter
)

set(
	SYSTEM_INCLUDE
	${SYSTEM_INCLUDE}
	#....
)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  
# # # # # # # # # # # # # # # # # CODES # # # # # # # # # # # # # # # # # # # #  
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  

file(GLOB PROJECT_SRC Testing/*.c*)
foreach(TESTING_SRC ${PROJECT_SRC})
	get_filename_component(FILE_NAME ${TESTING_SRC} NAME_WE)
	qt5_generate_moc(
		${TESTING_SRC}
		${FILE_NAME}.moc
	)
	add_executable(
		${FILE_NAME}
		${FILE_NAME}.moc
		${TESTING_SRC}
	)
	target_link_libraries(
		${FILE_NAME}
		${SYSTEM_LIBRARIES}
	)
	target_include_directories(
		${FILE_NAME}
		PUBLIC
		${SYSTEM_INCLUDE}
	)
	add_test(
		NAME
		${FILE_NAME}
		COMMAND
		${FILE_NAME}
	)
	#Grouping in MSVC
	source_group(
		"Generated Files"
		FILES
		${FILE_NAME}.moc
	)
	source_group(
		"Source Files" 
		FILES 
		${TESTING_SRC}
	)
endforeach()


