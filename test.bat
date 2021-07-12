@echo off
setlocal enabledelayedexpansion

REM Sample of a build file

SET timestamp=%TIME:~3,2%%TIME:~6,2%
SET args=-g -DDEBUG -D__WIN32__ -D_CRT_SECURE_NO_WARNINGS -Werror -Wall  -Wno-unused-function
SET include_path=-I D:\Guigui\Work\Prog\_include\ -I %VULKAN_SDK%\include

SET linker_options=-L D:\Guigui\Work\Prog\_lib -L %VULKAN_SDK%\lib -Xlinker -incremental:no
SET libs= -lShell32.lib -lvulkan-1.lib

REM Clear the temporary directory and create it if it doesn't exist
DEL /Q tmp > NUL 2> NUL

REM Build the main .exe here
clang %args% %include_path% test.c -o bin/test.exe %linker_options% %libs% -Xlinker -SUBSYSTEM:CONSOLE -Xlinker -PDB:tmp/test.pdb

ECHO BUILD OK