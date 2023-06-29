@echo off
cmake -B "vs2022_uwp" -S "./../" -G "Visual Studio 17 2022" -A x64 -DCMAKE_SYSTEM_NAME:String=WindowsStore -DCMAKE_SYSTEM_VERSION:String="10.0" -DCMAKE_INSTALL_PREFIX:String="SDK" %*
echo If cmake failed then be sure to check the "Universal Windows Platform development" checkbox in the Visual Studio Installer
echo Open vs2022_uwp\Alimer.sln to build the project.