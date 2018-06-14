# Requirements:

- Qt 5.8
- CMake
- Visual Studio 2015
- Google Test

if multiple Qt versions are installed then the best solution is to set the environment setting in the visual studio  PATH=PathToQt5.8_bin;%PATH%

# CMAKE Commands

cmake -G"Visual Studio 14 Win64" -DQT5_CMAKE_INCLUDE_DIR="C:\Qt\5.8\msvc2015_64\lib" -DBOOST_PATH="C:/Users/DeLlanoE/Documents/code/boost_1_64_0" -DRAPIDJSON_PATH="C:\Users\DeLlanoE\Documents\code\Adenita_thirdparty" ..
