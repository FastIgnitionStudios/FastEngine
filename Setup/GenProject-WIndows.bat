@echo off
pushd %~dp0\..\
call Setup\premake5.exe vs2022
popd
PAUSE