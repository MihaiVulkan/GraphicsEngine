
export BUILD_DIR="build"

cmake -G "Visual Studio 16 2019" -A x64 -S . -B $BUILD_DIR
cmake --build $BUILD_DIR --parallel 4