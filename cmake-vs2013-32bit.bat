del /s /q build
mkdir build
cd build
cmake -G "Visual Studio 12" -DCMAKE_INSTALL_PREFIX=../bin/win8-32 -DDHEWM3LIBS=../dhewm3-libs-master/i686-w64-mingw32 ../neo -DSDL2=OFF
pause