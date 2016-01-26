@echo off

set solution_path="G:\Cpp\Masterarbeit\Masterarbeit.sln"
set msbuild_path="C:\Windows\Microsoft.NET\Framework\v4.0.30319\"
set devenv_path="H:\Microsoft Visual Studio 10.0\Common7\IDE\"

C:
cd %msbuild_path%
msbuild.exe %solution_path% /t:clean /p:Configuration="Debug";Platform="Win32"
msbuild.exe %solution_path% /t:clean /p:Configuration="Debug";Platform="x64"
msbuild.exe %solution_path% /t:clean /p:Configuration="Release";Platform="Win32"
msbuild.exe %solution_path% /t:clean /p:Configuration="Release";Platform="x64"


echo.
echo Building...

H:
cd %devenv_path%

devenv.exe %solution_path% /Build "Debug|Win32"
devenv.exe %solution_path% /Build "Debug|x64"
devenv.exe %solution_path% /Build "Release|Win32"
devenv.exe %solution_path% /Build "Release|x64"


pause