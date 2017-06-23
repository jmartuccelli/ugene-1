# UGENE

## Building UGENE

### Prerequisites

Make sure the Qt (>= 5.2.1 and < 5.6) development libraries are installed:

* Ubuntu 16.04: `sudo apt-get install qt5-default qttools5-dev-tools qtscript5-dev libqt5svg5-dev libqt5webkit5-dev libprocps4-dev`
* Ubuntu 14.04: `sudo apt-get install qt5-default qttools5-dev-tools qtscript5-dev libqt5svg5-dev libqt5webkit5-dev libprocps3-dev`
* Ubuntu 12.04:
    * Download and install Qt 5.5.1: www.qt.io/download-open-source/
    * Set the system variable: export PATH=$PATH:~/Qt5.5.1/5.5/gcc_64/bin
    * `sudo apt-get install g++ libgl1-mesa-dev libproc-dev libglu1-mesa-dev`
    * `sudo ln -s /usr/lib/libproc.so /usr/lib/libprocps.so`
* Fedora:       `sudo yum install qt5-qtscript-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-linguist qt5-qtwebkit-devel gcc-c++ redhat-rpm-config procps-ng-devel mesa-libGLU-devel`
* Arch Linux:   `sudo pacman -S qt`
* Mac OS X: download from Qt official site (http://www.qt.io/download/)
* Windows: download from Qt official site (http://www.qt.io/download/)

### For Windows users:

To build with devenv (Visual Studio)

1. `qmake -r -tp vc ugene.pro`
2. open ugene.sln from Visual Studio or run `devenv.exe ugene.sln /Build` from MSVC command line

To build with nmake.exe:

1. `qmake -r ugene.pro`
2. run `nmake`, `nmake debug` or `nmake release` to build UGENE

Also you need to add "libeay.dll" and "ssleay.dll" from OpenSSL ToolKit to "../ugene/src/_debug" folder

### For *nix users:

0. installation paths may be set up in ugene_globals.pri
1. `qmake -r` (Fedora: `qmake-qt5 -r`)
2. `make -j 4`
3. `sudo make install`
4. `ugene -ui`

> Note: usually, `make` builds a release version of UGENE.
   However, on certain platforms default target is debug.
   To enforce release build use `make release` or `make all`.

Some more information you can see in installer/_common_data/README file.

### Build with CUDA

1. Download and install required software from http://www.nvidia.com/object/cuda_get.html for your OS
2. Make sure that some system variable are set:
   ```
   CUDA_LIB_PATH=/path_where_cuda_installed/lib
   CUDA_INC_PATH=/path_where_cuda_installed/include
   PATH=$PATH:/path_where_cuda_installed/bin
   ```
   
   for *nix: `LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_LIB_PATH`

3. cd ./src and open ugene_globals.pri, find and set variable UGENE_CUDA_DETECTED = 1

### Build with OpenCL

1. Download and install video driver wich supports OpenCL
2. Download OpenCL headers from http://www.khronos.org/registry/cl/
   or find them in video vendor SDK directory.
3. make sure that you have system variable is set correctly:
          `OPENCL_INC_PATH=/path_where_open_cl_installed`
4. cd ./src and open ugene_globals.pri, find and set variable `UGENE_OPENCL_DETECTED = 1`

> Notes: UGENE_CELL flag in ugene_globals should be uncommented when building on Cell BE platform
