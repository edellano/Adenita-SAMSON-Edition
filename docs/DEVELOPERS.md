# CMake

## Requirements:

- Qt 5.9.3
- CMake
- Visual Studio
- Boost library
- Rapidjson
- SAMSON SDK 0.7.0

# CMake Command

To create the developing environment use CMake:

```
mkdir build
cd build
cmake -G"Visual Studio 15 Win64" -DSAMSON_SDK_PATH="path/to/SAMSON/SDK/0.7.0" -DQT5_CMAKE_INCLUDE_DIR="path/to/Qt/5.9.3/msvc2017_64/lib" -DBOOST_PATH="path/to/boost_1_69_0" -DRAPIDJSON_PATH="path/to/rapidjson" ..```