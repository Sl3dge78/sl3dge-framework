#include <string.h>

#include "types.h"
#include "debug.h"

#define ARRAY_INIT_SIZE 10

// TODO : Test
struct Vector {
    void **buffer;
    u32 count;
    u32 capacity;
};


void array_init(Vector *v, u32 init_size) {
    v->buffer = (void **)malloc(sizeof(void*) * init_size);
    v->count = 0;
    v->capacity = init_size;
}

u32 array_count(Vector *v) {
    return v->count;
}

void array_destroy(Vector *v) {
    free(v->buffer);
    v->count = 0;
    v->capacity = 0;
}

void array_resize(Vector *v, u32 new_capacity) {
    void **new_buffer = (void**)realloc(v->buffer, new_capacity * sizeof(void*));
    if(new_buffer) {
        v->buffer = new_buffer;
        v->capacity = new_capacity;
    }
}

void array_add(Vector *v, void *item) {
    if(v->capacity == v->count) {
        array_resize(v, v->capacity * 2);
    }
    v->buffer[v->count] = item;
}

void *array_get(Vector *v, u32 index){
    if(index >= 0 && index < v->count){
        return v->buffer[index];
    }
    return NULL;
}


// TODO : Test
struct List {
    ListItem *head;
    ListItem *end;
    u32 size;
};

struct ListItem {
    void *data;
    ListItem *next;
    ListItem *prev;
};

internal void lldeleteitem(ListItem *item) {
    //free(item->data);
    free(item);
}

void llpushback(List *ll, void *object, size_t size) {
    ListItem *item = (ListItem *)malloc(sizeof(ListItem));
    //item->data = malloc(size);
    //memcpy(ll->head, object, size);
    item->data = object;
    if(ll->end){
        item->prev = ll->end;
    }

    if(!ll->head) {
        ll->head = item;
    } else {
        ll->end->next = item;
    }
    ll->end = item;
    ll->size++;
}

void llpopback(List *ll) {
    if(ll->head) {
        ListItem *last = ll->end->prev;
        lldeleteitem(ll->end);
        
        ll->size--;
        if(last) {
            ll->end = last;
        }
    }
}

void llpopfront(List *ll) {
    if(ll->head) {
        ListItem *first = ll->head->next;
        lldeleteitem(ll->head);
        ll->size--;
        if(first) {
            ll->head = first;
        
        }
    }
}

void llclear(List *ll){
    ListItem *i = ll->head;
    while(i != NULL) {
        ListItem *n = i->next;
        lldeleteitem(i);
        i = n;
    }
}