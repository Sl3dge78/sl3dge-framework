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

internal inline FILETIME Win32GetLastWriteTime(const char *file_name) {
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

internal bool Win32LoadModule(Module *module, const char *name) {
    SDL_Log("Loading module %s", name);

    u32 name_length = strlen(name);
    const char *path = "bin\\";
    const u32 path_length = strlen(path);

    u32 meta_length = path_length + name_length + strlen(".meta") + 1;
    module->meta_path = (char *)calloc(meta_length, sizeof(char));
    strcat_s(module->meta_path, meta_length, path);
    strcat_s(module->meta_path, meta_length, name);
    strcat_s(module->meta_path, meta_length, ".meta");

    u32 orig_length = path_length + name_length + strlen(".dll") + 1;
    char *orig_dll = (char *)calloc(orig_length, sizeof(char));
    strcat_s(orig_dll, orig_length, path);
    strcat_s(orig_dll, orig_length, name);
    strcat_s(orig_dll, orig_length, ".dll");

    u32 temp_length = path_length + name_length + strlen("_temp.dll") + 1;
    module->tmp_dll = (char *)calloc(temp_length, sizeof(char));
    strcat_s(module->tmp_dll, temp_length, path);
    strcat_s(module->tmp_dll, temp_length, name);
    strcat_s(module->tmp_dll, temp_length, "_temp.dll");

    // Copy the .dll to the new _temp.dll to allow writing by the compiler, only
    // if it exists. CopyFile deletes the destination even on failure, so do the
    // copy only if the file exists. This will reload the previous _tmp
    WIN32_FIND_DATA f;
    HANDLE hdl = FindFirstFile(orig_dll, &f);
    if (hdl != INVALID_HANDLE_VALUE) {
        FindClose(hdl);
        DeleteFile(module->tmp_dll);
        if (!CopyFile(orig_dll, module->tmp_dll, FALSE)) {
            DWORD dw = GetLastError();
            SDL_LogError(0, "Error copying module %s, %d", name, dw);
        }
    } else {
        SDL_LogWarn(0, "%s.dll doesn't exist, reloading the previous module.",
                    name);
    }

    module->dll = LoadLibrary(module->tmp_dll);

    if (!module->dll) {
        SDL_LogError(0, "Unable to load module %s", name);
        return false;
    }

    free(orig_dll);
    module->last_write_time = Win32GetLastWriteTime(module->meta_path);

    SDL_Log("Module loaded : %s", name);
    return true;
}

internal void Win32CloseModule(Module *module) {
    FreeLibrary(module->dll);
    free(module->tmp_dll);
    free(module->meta_path);
}
#endif // SL_MODULE_H
