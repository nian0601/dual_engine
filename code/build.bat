@echo off

call "../misc/shell.bat"

mkdir ..\workbed
pushd ..\workbed

SET CL_OPTS=-Zi
SET LINK_OPTS= /NOLOGO /OUT:application.exe user32.lib gdi32.lib opengl32.lib glu32.lib
SET CODE_PATH=c:\_GIT\dual_engine\code\main.cpp

cl %CL_OPTS% %CODE_PATH% /link %LINK_OPTS%

popd
