/* date = June 12th 2021 11:28 am */

#ifndef SL_MODULE_H
#define SL_MODULE_H

#include <windows.h>

// SAMPLE
/*

// Load module
Module game_module = {};
    Win32LoadModule(&game_module, "game");
    // > Load pfn

// Check for changes - In main loop :
if(Win32ShouldReloadModule(&game_module)) {
            Win32CloseModule(&game_module);
            Win32LoadModule(&game_module, "game");
            // > Load pfn
         }

// Close module
Win32CloseModule(&game_module);

*/

typedef struct Module {
    char *meta_path;
    FILETIME last_write_time;
    HMODULE dll;
    char *tmp_dll;
} Module;

internal inline FILETIME Win32GetLastWriteTime(char *file_name) {
    FILETIME last_write_time = {};

    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(file_name, &data);
    if (handle != INVALID_HANDLE_VALUE) {
        last_write_time = data.ftLastWriteTime;
        FindClose(handle);
    }

    return last_write_time;
}

internal bool Win32ShouldReloadModule(Module *module) {
    FILETIME current_time = Win32GetLastWriteTime(module->meta_path);
    if (CompareFileTime(&module->last_write_time,
                        &current_time)) { // Has the file changed ?
        module->last_write_time = current_time;
        return true;
    }
    return false;
}

internal void Win32LoadModule(Module *module, const char *name) {
    SDL_Log("Loading module %s", name);

    u32 name_length = strlen(name);
    const char *path = "bin\\";
    const u32 path_length = strlen(path);

    module->meta_path = (char *)calloc(
        path_length + name_length + strlen(".meta") + 1, sizeof(char));
    strcat(module->meta_path, path);
    strcat(module->meta_path, name);
    strcat(module->meta_path, ".meta");

    char *orig_dll = (char *)calloc(
        path_length + name_length + strlen(".dll") + 1, sizeof(char));
    strcat(orig_dll, path);
    strcat(orig_dll, name);
    strcat(orig_dll, ".dll");
    module->tmp_dll = (char *)calloc(
        path_length + name_length + strlen("_temp.dll") + 1, sizeof(char));
    strcat(module->tmp_dll, path);
    strcat(module->tmp_dll, name);
    strcat(module->tmp_dll, "_temp.dll");
    CopyFile(orig_dll, module->tmp_dll, FALSE);
    module->dll = LoadLibrary(module->tmp_dll);

    if (!module->dll) {
        SDL_LogError(0, "Unable to load module %s", name);
    }

    free(orig_dll);
    module->last_write_time = Win32GetLastWriteTime(module->meta_path);

    SDL_Log("Module loaded : %s", name);
}

internal void Win32CloseModule(Module *module) {
    FreeLibrary(module->dll);
    DeleteFile(module->tmp_dll);
    free(module->tmp_dll);
    free(module->meta_path);
}
#endif // SL_MODULE_H
