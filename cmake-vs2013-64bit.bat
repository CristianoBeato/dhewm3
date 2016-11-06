del /s /q build
mkdir build
cd build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=../bin/win8-64 -DDHEWM3LIBS=../dhewm3-libs-master/x86_64-w64-mingw32 ../neo
pause