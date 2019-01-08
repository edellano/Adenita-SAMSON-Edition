# Requirements:

- Qt 5.8
- CMake
- Visual Studio 2015
- Google Test

if multiple Qt versions are installed then the best solution is to set the environment setting in the visual studio  PATH=PathToQt5.8_bin;%PATH%

# CMAKE Commands

cmake -G"Visual Studio 14 Win64" -DSAMSON_SDK_PATH="C:\Users\DeLlanoE\NANO-D\SDK\0.7.0" -DQT5_CMAKE_INCLUDE_DIR="C:\Qt\5.8\msvc2015_64\lib" -DBOOST_PATH="C:/Users/DeLlanoE/Documents/code/boost_1_64_0" -DRAPIDJSON_PATH="C:\Users\DeLlanoE\Documents\code\Adenita_thirdparty\rapidjson"

cmake -G"Visual Studio 14 Win64" -DSAMSON_SDK_PATH="C:\Users\Haichao\NANO-D\SDK\0.7.0" -DQT5_CMAKE_INCLUDE_DIR="C:\Qt\Qt5.8.0\5.8\msvc2015_64\lib" -DBOOST_PATH="C:/Development/boost_1_63_0" -DRAPIDJSON_PATH="C:\Development\DNA_Nanomodeling\Adenita\Adenita\thirdparty" ..


# short cuts

CTRL + O 	Open file 
CTRL + S	Save file 	
CTRL + F 	Search
CTRL + E 	Export sequence
CTRL + R 	Create new structres (DNA origami, DNA wireframe)
CTRL + ; 	Set scaffold
CTRL + B 	Activate break mode 
CTRL + [ 	Activate connecting ssDNA mode
CTRL + ]	Activate connecting dsDNA mode 	
CTRL + M	Activate mutating nucleotide/amino acid mode
CTRL + I	Activate inserting nucleotide/amino acid mode
CTRL + D	Activate deleting nucleotide/amino acid mode 	
CTRL + L 	Activate adding loop 
CTRL + K	Activate adding skip 	
CTRL + M	Map to amino acid 	
CTRL + .	Edit all-atom model 	
CTRL + P	Paint dsDNA / ssDNA / nucleotide / atoms