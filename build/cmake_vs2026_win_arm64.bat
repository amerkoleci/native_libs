@echo off
cmake -B "build_vs2026_win_arm64" -S "./../" -G "Visual Studio 17 2022" -A ARM64 -DCMAKE_INSTALL_PREFIX="win-arm64-sdk" %*
echo Open build_vs2026_win_arm64\Alimer.sln to build the project.