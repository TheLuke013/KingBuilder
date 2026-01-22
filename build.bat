@echo off

set DIR=build
set TARGET=%DIR%/KingBuilder.exe

rmdir /s /q %DIR% 2>nul
mkdir %DIR% 2>nul

g++ -o %TARGET% src/*.cpp -Ithirdparty/rapidjson/include --std=c++17 -static

pause