# Qvtk Project
A medical imaged assisted surgical guidance robotic system

## Overview
The Qvtk Project is a software designed for medical image assisted surgical robotic system.

## Prerequisites
Known good build dependecies:

- [CMake](https://cmake.org/download/)
- [Qt 5.10.1](https://www.qt.io/download)
- [VTK 7.1.1](https://github.com/Kitware/VTK/tree/v7.1.1)
- [ITK 4.13.0](https://github.com/InsightSoftwareConsortium/ITK/tree/v4.13.0)
- [OpenCV 3.4.1](https://github.com/opencv/opencv/tree/3.4.1)

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
- [Dental](https://gitlab.com/Qvtk/QvtkDental/tags)
- [Orthopedics (Under development)]()
- [Neural (Under development)]() 

## Version
Qvtk Project v0.0.1a

***FOR INTERNAL USE ONLY***

## License
This project is licensed under the Qvtk Redistribution (?????) License - see the [LICENSE.md](/LICENSE.md) file for details

## Disclaimers

## Acknowledgments
