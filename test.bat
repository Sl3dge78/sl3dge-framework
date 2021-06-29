@echo off
setlocal enabledelayedexpansion

REM Sample of a build file

CLS

SET timestamp=%TIME:~3,2%%TIME:~6,2%
SET args=-g -DDEBUG
SET include_path=-I D:\Guigui\Work\Prog\_include\ -I %VULKAN_SDK%\include

SET linker_options=-L D:\Guigui\Work\Prog\_lib -L %VULKAN_SDK%\lib -Xlinker -incremental:no
SET libs=-lSDL2main.lib -lSDL2.lib -lSDL2_image.lib -lShell32.lib -lvulkan-1.lib

REM Clear the temporary directory and create it if it doesn't exist
DEL /Q tmp > NUL 2> NUL
MKDIR tmp > NUL 2> NUL

MKDIR bin

REM Build the main .exe here
clang %args% %include_path% src/test.c -o bin/test.exe %linker_options% %libs% -Xlinker -SUBSYSTEM:WINDOWS -Xlinker -PDB:tmp/test.pdb

CALL bin/test.exe