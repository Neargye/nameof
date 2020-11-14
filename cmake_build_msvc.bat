if exist build RMDIR /S /Q build
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
ctest -V --output-on-failure -C Debug
pause
