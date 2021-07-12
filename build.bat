@echo off
setlocal enabledelayedexpansion

REM Sample of a build file

CLS

SET timestamp=%TIME:~3,2%%TIME:~6,2%
SET args=-g -DDEBUG  -Wextra -pedantic
SET include_path=-I D:\Guigui\Work\Prog\_include\ -I %VULKAN_SDK%\include

SET linker_options=-L D:\Guigui\Work\Prog\_lib -L %VULKAN_SDK%\lib -Xlinker -incremental:no
SET libs=-lSDL2main.lib -lSDL2.lib -lSDL2_image.lib -lShell32.lib -lvulkan-1.lib

REM Build the main .exe here
ECHO win32.cpp
clang %args% %include_path% src/win32.cpp -o bin/win32.exe %linker_options% %libs% -Xlinker -SUBSYSTEM:WINDOWS -Xlinker -PDB:tmp/win32.pdb
IF !ERRORLEVEL! == 0 (
    ECHO Ok!
)

REM build each module here
REM it's looking for a file named with the same name as the module in src/
REM Will create a dll and a .meta file once build is complete
FOR %%G IN (module1,module2) DO (
    SET timestamp=%TIME:~3,2%%TIME:~6,2%

    REM Clear the temporary directory
    PUSHD tmp\
    DEL /Q %module_name%_*.pdb  2> NUL > NUL
    POPD

    ECHO %%G

    clang %args% %include_path% -shared src/%%G.cpp -o bin/%%G.dll %linker_options% %libs% -Xlinker -PDB:tmp/%%G_%timestamp%.pdb -Xlinker -IMPLIB:tmp/%%G.lib
    if !ERRORLEVEL! == 0 (
        ECHO a > bin/%%G.meta
        ECHO BUILD OK
    )
)

EXIT -b %ERRORLEVEL%