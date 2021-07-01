#ifndef SDEBUG_H
#define SDEBUG_H

#include <stdio.h>

#include "sTypes.h"
#include "sLogging.h"

#if defined(DEBUG)

#define ASSERT(expression)                                                                         \
    if(!(expression)) {                                                                            \
        __builtin_trap();                                                                          \
    }
#define ASSERT_MSG(expression, msg)                                                                \
    if(!(expression)) {                                                                            \
        sError(msg);                                                                               \
        __builtin_trap();                                                                          \
    }

#define HANG(expression)                                                                           \
    if(!(expression))                                                                              \
        while(1)                                                                                   \
            ;

#define HANG_MSG(expression, msg)                                                                  \
    if(!(expression))                                                                              \
        sError(msg);                                                                               \
    while(1)                                                                                       \
        ;

global bool DBG_keep_console_open;

#define KEEP_CONSOLE_OPEN(value) DBG_keep_console_open |= value

#define DBG_END() PerformEndChecks()

typedef struct MemoryInfo {
    void *ptr;
    size_t size;
    const char *file;
    u32 line;
} MemoryInfo;

typedef struct MemoryLeak {
    MemoryInfo info;
    struct MemoryLeak *next;
} MemoryLeak;

global MemoryLeak *array_start = NULL;
global MemoryLeak *array_end = NULL;

internal void add_memory_info(void *ptr, size_t size, const char *filename, u32 line) {
    MemoryLeak *leak = (MemoryLeak *)malloc(sizeof(MemoryLeak));

    leak->info.ptr = ptr;
    leak->info.size = size;
    leak->info.file = filename;
    leak->info.line = line;
    leak->next = NULL;

    if(array_start == NULL) {
        array_start = leak;
        array_end = leak;
    } else {
        array_end->next = leak;
        array_end = leak;
    }
}

internal void delete_memory_info(void *ptr) {
    ASSERT_MSG(array_start, "Attempting to free a nullptr!");

    if(array_start->info.ptr == ptr) {
        MemoryLeak *leak = array_start;
        array_start = array_start->next;

        free(leak);
        return;
    }

    for(MemoryLeak *leak = array_start; leak != NULL; leak = leak->next) {
        MemoryLeak *next_leak = leak->next;
        ASSERT_MSG(next_leak, "Attempting to free a nullptr!");

        if(next_leak->info.ptr == ptr) { // Si il faut supprimer le suivant
            if(array_end == next_leak) {
                leak->next = NULL;
                array_end = leak;
            } else {
                leak->next = next_leak->next;
            }
            free(next_leak);
            return;
        }
    }
}

internal void clear_array() {
    MemoryLeak *leak = array_start;
    MemoryLeak *to_delete = array_start;

    while(leak != NULL) {
        leak = leak->next;
        free(to_delete);
        to_delete = leak;
    }
}

void *DBG_malloc(size_t size, const char *filename, u32 line) {
    void *ptr = malloc(size);
    if(ptr != NULL) {
        add_memory_info(ptr, size, filename, line);
    }
    return ptr;
}

void *DBG_calloc(size_t num, size_t size, const char *filename, u32 line) {
    void *ptr = calloc(num, size);
    if(ptr != NULL) {
        add_memory_info(ptr, num * size, filename, line);
    }
    return ptr;
}

void *DBG_realloc(void *ptr, size_t new_size, const char *filename, u32 line) {
    void *new_ptr = realloc(ptr, new_size);
    if(new_ptr != NULL) {
        if(ptr != NULL)
            delete_memory_info(ptr);

        add_memory_info(new_ptr, new_size, filename, line);
    }
    return new_ptr;
}

void DBG_free(void *ptr) {
    delete_memory_info(ptr);
    free(ptr);
}

bool DBG_DumpMemoryLeaks() {
    int count = 0;
    for(MemoryLeak *leak = array_start; leak != NULL; leak = leak->next) {
        sError("Memory leak found - Address: %p | Size: %06d | Last alloc: %s:%d",
               leak->info.ptr,
               (u64)leak->info.size,
               leak->info.file,
               leak->info.line);
        count++;
    }
    clear_array();
    return count;
}

void PerformEndChecks() {
    DBG_keep_console_open |= DBG_DumpMemoryLeaks();

    if(DBG_keep_console_open) {
        while(1)
            ;
    }
}

#define sMalloc(size) DBG_malloc(size, __FILE__, __LINE__)
#define sCalloc(num, size) DBG_calloc(num, size, __FILE__, __LINE__)
#define sRealloc(ptr, size) DBG_realloc(ptr, size, __FILE__, __LINE__)
#define sFree(ptr) DBG_free(ptr)

#else // #if DEBUG

#define ASSERT(expression)
#define ASSERT_MSG(expression, msg)
#define HANG(expression)
#define HANG_MSG(expression, msg)
#define KEEP_CONSOLE_OPEN(value)
#define DBG_END()

#define smalloc(size) malloc(size)
#define scalloc(num, size) calloc(num, size)
#define srealloc(ptr, size) realloc(ptr, size)
#define sfree(ptr) free(ptr)

#endif // #if DEBUG

#endif // #ifdef SL_DEBUG_H