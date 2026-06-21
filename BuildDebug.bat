cmake -S . -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 exit /b %errorlevel%

cmake --build build-debug
