@echo off

set solution_path="G:\Cpp\Masterarbeit\Masterarbeit.sln"
set msbuild_path="C:\Windows\Microsoft.NET\Framework\v4.0.30319\"

C:
cd %msbuild_path%
msbuild.exe %solution_path% /t:clean /p:Configuration="Debug";Platform="Win32"
msbuild.exe %solution_path% /t:clean /p:Configuration="Debug";Platform="x64"
msbuild.exe %solution_path% /t:clean /p:Configuration="Release";Platform="Win32"
msbuild.exe %solution_path% /t:clean /p:Configuration="Release";Platform="x64"
pause