# Qvtk Project
A medical imaged assisted surgical guidance robotic system

## Overview
The Qvtk Project is a software designed for medical image assisted surgical robotic system.

## Prerequisites
Known good build dependecies:

<h2><a href="https://git-scm.com/downloads">Git-2.13.3</a></h2>
<p>
  Following the Git installation wizard to install git binary<br>
</p>
<h2><a href="https://cmake.org/download/">CMake-3.9.0</a></h2>
<p>
  Following the CMake installation wizard to install CMake binary<br>
</p>
<h2><a href="https://download.qt.io/official_releases/qt/5.10/5.10.1/">Qt-5.10.1</a></h2>
<p>
  Following the Qt installation wizard to install Qt binary<br>
</p>
<h2><a href="https://github.com/Kitware/VTK">VTK-7.1.1</a></h2>
<p>
  <ul>
    <li>CMAKE_CXX_MP_FLAG:BOOL=ON<br></li>
    <li>BUILD_DOCUMENTATION:BOOL=OFF<br></li>
    <li>BUILD_EXAMPLES:BOOL=OFF<br></li>
    <li>BUILD_SHARED_LIBS:BOOL=ON<br></li>
    <li>BUILD_TESTING:BOOL=OFF<br></li>
    <li>Module_vtkImagingOpenGL2:BOOL=ON<br></li>
    <li>Module_vtkIOExportOpenGL2:BOOL=ON<br></li>
    <li>Module_vtkRenderingLICOpenGL2:BOOL=ON<br></li>
    <li>VTK_QT_VERSION:STRING=5<br></li>
    <li>VTK_Group_Qt:BOOL=ON<br></li>
    <li>Module_vtkGUISupportQt:BOOL=ON<br></li>
    <li>Module_vtkGUISupportQtOpenGL:BOOL=ON<br></li>
    <li>Module_vtkGUISupportQtSQL:BOOL=ON<br></li>
    <li>Module_vtkGUISupportQtWebkit:BOOL=OFF<br></li>
    <li>Qt5_DIR:PATH=${Qt5_DIR}<br></li>
    <li>Qt5Core_DIR:PATH=${Qt5_DIR}Core<br></li>
    <li>Qt5Gui_DIR:PATH=${Qt5_DIR}Gui<br></li>
    <li>Qt5OpenGL_DIR:PATH=${Qt5_DIR}OpenGL<br></li>
    <li>Qt5Sql_DIR:PATH=${Qt5_DIR}Sql<br></li>
    <li>Qt5UiPlugin_DIR:PATH=${Qt5_DIR}UiPlugin<br></li>
    <li>Qt5Widgets_DIR:PATH=${Qt5_DIR}Widgets<br></li>
  </ul>
</p>
<h2><a href="https://github.com/Kitware/ITK">ITK-4.13.0</a></h2>
<p>
  <ul>
    <li>CMAKE_CXX_MP_FLAG:BOOL=ON<br></li>
    <li>BUILD_DOCUMENTATION:BOOL=OFF<br></li>
    <li>BUILD_EXAMPLES:BOOL=OFF<br></li>
    <li>BUILD_SHARED_LIBS:BOOL=ON<br></li>
    <li>BUILD_TESTING:BOOL=OFF<br></li>
    <li>Module_ITKVtkGlue:BOOL=ON<br></li>
    <li>VTK_DIR:PATH=${CMAKE_BINARY_DIR}/VTK-build<br></li>
    <li>Module_ITKReview:BOOL=ON<br></li>
  </ul>
</p>
<h2><a href="https://github.com/commontk/CTK">CTK-524fd8729bbee740392739d22f64784ec81a9804</a></h2>
<p>
  <ul>
    <li>CMAKE_CXX_MP_FLAG:BOOL=ON<br></li>
    <li>BUILD_DOCUMENTATION:BOOL=OFF<br></li>
    <li>CTK_BUILD_EXAMPLES:BOOL=OFF<br></li>
    <li>CTK_BUILD_SHARED_LIBS:BOOL=ON<br></li>
    <li>BUILD_TESTING:BOOL=OFF<br></li>
    <li>CTK_ENABLE_DICOM:BOOL=ON<br></li>
    <li>CTK_LIB_DICOM/Core:BOOL=ON<br></li>
    <li>CTK_LIB_DICOM/Widgets:BOOL=ON<br></li>
    <li>CTK_QT_VERSION:STRING=5<br></li>
    <li>Qt5_DIR:PATH=${Qt5_DIR}<br></li>
  </ul>
</p>
<h2><a href="https://github.com/opencv/opencv">OpenCV-3.3.0</a></h2>
<p>
  <ul>
    <li>CMAKE_CXX_MP_FLAG:BOOL=ON<br></li>
    <li>BUILD_DOCS:BOOL=OFF<br></li>
    <li>BUILD_EXAMPLES:BOOL=OFF<br></li>
    <li>BUILD_TESTS:BOOL=OFF<br></li>
    <li>BUILD_SHARED_LIBS:BOOL=ON<br></li>
    <li>BUILD_WITH_STATIC_CRT:BOOL=ON<br></li>
    <li>BUILD_opencv_java:BOOL=OFF<br></li>
    <li>BUILD_opencv_python3:BOOL=OFF<br></li>
    <li>WITH_MATLAB:BOOL=OFF<br></li>
    <li>WITH_CUDA:BOOL=OFF<br></li>
  </ul>
</p>

Build pass on Windows 10 with MSVC 2015. Test on your own on other platforms and compilers.

## Compile from Source
1. ```git clone --recursive -j12 git@gitlab.com:Qvtk/QvtkProject.git ```, ```-j12``` refers to the number of parallel submodule cloning.
2. CMake Configure
3. Provide denendencies upon request
3. Select suitable Qvtk build projects
4. CMake Generate
3. Compile the project

The project compilation is only tested with MSVC 2015 on Windows 10. Try on your own with other platforms and compilers.

If you only clone the repository and found submodules missing, use following command: 
```git submodule update --init --recursive```

To update local repository, use ```git submodule foreach --recursive git pull origin master```

## Latest Binary

## Version
Qvtk Project v0.0.1a

***FOR INTERNAL USE ONLY***

## License
This project is licensed under the Qvtk Redistribution (?????) License - see the [LICENSE.md](/LICENSE.md) file for details

## Disclaimers

## Acknowledgments
