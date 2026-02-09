@echo off
cmake -B "build_vs2026_win_x64" -S "./../" -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX="win-x64-sdk" %*
echo Open build_vs2026_win_x64\Alimer.sln to build the project.
