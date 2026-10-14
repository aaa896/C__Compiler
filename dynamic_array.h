#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#define ARRAY_CAPACITY_DEFAULT 8

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "tdf.h"


typedef struct {
    size_t count;
    size_t capacity;
}Array_Header;


#define hash_function(key, key_size, return_value) do{\
  return_value = 2166136261u;\
  for (int i = 0; i < key_size; ++i) {\
    return_value ^= ((uint8_t *)key)[i];\
    return_value *= 16777619;\
  }\
}while(0)


#define get_array_header(array)   ((array) ? ((Array_Header*)(((uint8_t*)(array))  - sizeof(Array_Header))) : 0)
#define get_array_capacity(array)   ((array) ? ((Array_Header*)(((uint8_t*)(array))  - sizeof(Array_Header)))->capacity : 0)
#define get_array_count(array)   ((array) ? ((Array_Header*)(((uint8_t*)(array))  - sizeof(Array_Header)))->count : 0)

#define array_create(array, cap)\
    do {\
        int item_size =  sizeof((*array)[0]);\
        int header_space = sizeof(Array_Header) ;\
        int array_size = cap * item_size;\
        Array_Header *header_data = (Array_Header*)malloc(array_size + header_space);\
        if (!header_data) assert(0 && "create array malloc fail");\
        memset(header_data, 0, array_size + header_space);\
        header_data->capacity = cap;\
        (*array) = (void*)((uint8_t*)header_data + header_space);\
    } while (0);


#define array_append(array, item)\
    do {\
        if ((*array) == 0) {\
            array_create(array, ARRAY_CAPACITY_DEFAULT);\
        }\
        int header_space = sizeof(Array_Header) ;\
        Array_Header *header = (Array_Header*)((uint8_t*)((*array))  - header_space);\
        if (header->count == header->capacity) {\
            header->capacity *= 2;\
            int new_size =  header->capacity * sizeof((*array)[0]) + header_space;\
            header = (Array_Header*)realloc(header, new_size);\
            if (!header) assert(0 && "array realloc fail\n");\
            (*array) = (void*)((uint8_t*)header + header_space);\
        }\
        (*array)[header->count] = item;\
        header->count += 1;\
    }while(0);


#define array_pop(array, return_value)\
    do{\
        Array_Header *_header  get_array_header(array);\
        if (_header->count == 0) {\
            ASSERT(0);\
        }\
        return_value = array[_header->count -1];\
        _header->count -=1;\
    }while(0)


#define array_reserve_items(array, item_count)\
    do {\
        if ((*array) == 0) {\
            array_create(array, ARRAY_CAPACITY_DEFAULT);\
        }\
        int header_space = sizeof(Array_Header) ;\
        Array_Header *header = (Array_Header*)((uint8_t*)((*array))  - header_space);\
        int new_count = header->count + item_count;\
        if (new_count > header->capacity) {\
            int old_capacity = header->capacity;\
            while (new_count > header->capacity){ \
                header->capacity *= 2;\
            }\
            int old_size =  old_capacity * sizeof((*array)[0]) + header_space;\
            int new_size =  header->capacity * sizeof((*array)[0]) + header_space;\
            header = (Array_Header*)realloc(header, new_size);\
            if (!header) assert(0 && "array realloc fail\n");\
            memset((uint8_t*)header + old_size , 0, new_size - old_size);\
            (*array) = (void *)((uint8_t*)header + header_space);\
        }\
    }while(0);

#define array_add_size_count(array, size) \
    do{\
        Array_Header *_header  =get_array_header(*array);\
        if (_header->count - size < 0) {\
            ASSERT(0);\
        }\
        array_reserve_items(array, size);\
        _header->count +=size;\
    }while(0)


#define array_free(array)\
    do{\
        Array_Header *header = ((Array_Header*)(*array)) - 1;\
        free(header);\
        (*array) = 0;\
    }while(0)


#define array_insert(array, index, item) \
do{\
    ASSERT(*array);\
    ASSERT(index >= 0);\
    Array_Header *header = ((Array_Header*)((uint8_t*)((*array))  - sizeof(Array_Header)));\
    size_t count = header->count;\
    if (count <= index) ASSERT(0);\
    if (count == header->capacity) {\
        array_append(array, item);\
        header->count -=1;\
    }\
    for (int i = count -1; i >= index; --i) {\
        (*array)[i +1] = (*array)[i];\
    }\
    header->count +=1;\
    (*array)[index] = item;\
}while(0)

typedef struct{
    bool *tombstone;
    bool *occupied;
    size_t count;
    size_t capacity;
}Hash_Map_Header;



#define HASH_DEFAULT_CAPACITY 8
#define HASH_INCREASE_PERCENT 0.6

#define get_hm_header(array)   ((array) ? ((Hash_Map_Header*)(((uint8_t*)(array))  - sizeof(Hash_Map_Header))) : 0)
#define get_hm_capacity(array)   ((array) ? ((Hash_Map_Header*)(((uint8_t*)(array))  - sizeof(Hash_Map_Header)))->capacity : 0)
#define get_hm_count(array)   ((array) ? ((Hash_Map_Header*)(((uint8_t*)(array))  - sizeof(Hash_Map_Header)))->count : 0)

#define hash_free(hm) do {\
    Hash_Map_Header *_header = get_hm_header(*hm);\
    if (_header->occupied)\
        array_free(&_header->occupied);\
    if (_header->tombstone)\
        array_free(&_header->tombstone);\
    free(_header);\
    *hm = 0;\
}while(0)


#define hash_create(hm, item_size, cap)\
    do {\
        int _header_space = sizeof(Hash_Map_Header) ;\
        int _hm_size = cap * item_size;\
        Hash_Map_Header *_header_data = (Hash_Map_Header*)malloc(_hm_size + _header_space);\
        if (!_header_data) assert(0 && "create hm malloc fail");\
        memset(_header_data, 0, _hm_size + _header_space);\
        (*hm) = (void*)(((uint8_t*)_header_data) + _header_space);\
        _header_data->capacity = cap;\
    } while (0);



#define hash_add(hm, item) do{\
\
    if (!(*hm)) {\
        hash_create(hm, sizeof((*hm)[0]), HASH_DEFAULT_CAPACITY);\
        Hash_Map_Header *_header =get_hm_header(*hm);\
        array_create(&_header->occupied, HASH_DEFAULT_CAPACITY);\
        array_create(&_header->tombstone, HASH_DEFAULT_CAPACITY);\
\
    }\
\
    int _cap = get_hm_capacity(*hm);\
    int _count = get_hm_count(*hm);\
    float _fill_percent = (float)(_count + 1)/ (float)_cap;\
    if (_fill_percent >= HASH_INCREASE_PERCENT) {\
        int _new_cap = _cap * 2;\
        Hash_Map_Header *_hm_header = get_hm_header(*hm);\
        void *_hm_new;\
        int _item_size = sizeof((*hm)[0]);\
        hash_create(&_hm_new, _item_size, _new_cap);\
        bool *_new_occupied ;\
        array_create(&_new_occupied, _new_cap);\
        bool *_new_tombstone ;\
        array_create(&_new_tombstone, _new_cap);\
        for (int _old_i   = 0; _old_i < _cap; ++_old_i ) {\
            if (_hm_header->occupied[_old_i]) {\
                unsigned int _new_i = 0 ;\
                hash_function(&(*hm)[_old_i].key, sizeof((*hm)[_old_i].key), _new_i) ;\
                _new_i %= _new_cap;\
                while (1) {\
                    if (!_new_occupied[_new_i]) {\
                        memcpy((uint8_t*)_hm_new + (_item_size * _new_i),  &(*hm)[_old_i], _item_size);\
                        _new_occupied[_new_i] = true;\
                        _new_tombstone[_new_i] = false;\
                        break;\
                    }\
\
                    if (_new_i == _new_cap)\
                        _new_i = 0;\
                    ++_new_i;\
                }\
\
            }\
        }\
\
        void *_old_ptr = *hm;\
        *hm  = _hm_new;\
        hash_free(&_old_ptr);\
        _cap = get_hm_capacity(*hm);\
        _count = get_hm_count(*hm);\
        Hash_Map_Header *_header= get_hm_header(*hm);\
        _header->occupied = _new_occupied;\
        _header->tombstone = _new_tombstone;\
\
    }\
\
    Hash_Map_Header *_hm_header = get_hm_header(*hm);\
    unsigned int _index;\
    hash_function(&item.key, sizeof(item.key), _index); \
    _index %= _cap;\
    while (1) {\
        if (item.key == (*hm)[_index].key ) {\
            bool _tombstone = _hm_header->tombstone[_index];\
            (*hm)[_index] = item;\
            _hm_header->occupied[_index] = true;\
            _hm_header->tombstone[_index] = _tombstone;\
            break;\
        } else if (!_hm_header->occupied[_index]) {\
            bool _tombstone = _hm_header->tombstone[_index];\
            (*hm)[_index] = item;\
            _hm_header->occupied[_index] = true;\
            _hm_header->tombstone[_index] = _tombstone;\
            _hm_header->count +=1;\
            break;\
        }\
\
        ++_index;\
        if (_index == _cap)\
            _index = 0;\
    }\
\
\
}while(0)

#define hash_find(hm, item, return_value) do{\
    int _cap = get_hm_capacity(hm);\
    unsigned int _index;\
    hash_function(&item.key, sizeof(item.key), _index);\
    _index %= _cap;\
    return_value= -1;\
    Hash_Map_Header *_hm_header = get_hm_header(hm);\
    for (int search_count = 0; search_count < _cap; ++search_count) {\
        if (_hm_header->occupied[_index]) {\
            if (hm[_index].key == item.key) {\
                return_value = _index;\
                break;\
            }\
        }else if (!_hm_header->tombstone[_index]) {\
            break;\
        }\
\
        _index += 1;\
        if (_index == _cap)\
            _index = 0;\
\
    }\
}while(0)

#define hash_remove(hm, item, return_value) do{\
    int _cap = get_hm_capacity(hm);\
    unsigned int _index;\
    hash_function(&item.key, sizeof(item.key),_index);\
    _index %= _cap;\
\
    return_value = false;\
    Hash_Map_Header *_hm_header = get_hm_header(hm);\
    for (int search_count = 0; search_count < _cap; ++search_count) {\
        if (_hm_header->occupied[_index]) {\
            if (hm[_index].key == item.key) {\
                _hm_header->occupied[_index] = false;\
                _hm_header->tombstone[_index] = true;\
                _hm_header->count -=1;\
                return_value = true;\
                break;\
            }\
        }else if (!_hm_header->tombstone[_index]) {\
            break;\
        }\
        \
        _index += 1;\
        if (_index == _cap)\
        _index = 0;\
        \
    }\
}while(0)

#define cstr_hash_free(hm) do {\
    Hash_Map_Header *_header = get_hm_header(*hm);\
    for (int i = 0; i < _header->capacity; ++i) {\
        if ((*hm)[i].key)\
            free((*hm)[i].key);\
    }\
    hash_free(hm);\
}while(0)




#define cstr_hash_add(hm, item) do{\
\
    if (!(*hm)) {\
        hash_create(hm, sizeof((*hm)[0]), HASH_DEFAULT_CAPACITY);\
        Hash_Map_Header *_header =get_hm_header(*hm);\
        array_create(&_header->occupied, HASH_DEFAULT_CAPACITY);\
        array_create(&_header->tombstone, HASH_DEFAULT_CAPACITY);\
\
    }\
\
    int _cap = get_hm_capacity(*hm);\
    int _count = get_hm_count(*hm);\
    float _fill_percent = (float)(_count + 1)/ (float)_cap;\
    if (_fill_percent >= HASH_INCREASE_PERCENT) {\
        int _new_cap = _cap * 2;\
        Hash_Map_Header *_hm_header = get_hm_header(*hm);\
        void *_hm_new;\
        int _item_size = sizeof((*hm)[0]);\
        hash_create(&_hm_new, _item_size, _new_cap);\
        bool *_new_occupied ;\
        array_create(&_new_occupied, _new_cap);\
        bool *_new_tombstone ;\
        array_create(&_new_tombstone, _new_cap);\
        for (int _old_i   = 0; _old_i < _cap; ++_old_i ) {\
            if (_hm_header->occupied[_old_i]) {\
                unsigned int _new_i = 0 ;\
                hash_function((*hm)[_old_i].key, strlen((*hm)[_old_i].key), _new_i) ;\
                _new_i %= _new_cap;\
                while (1) {\
                    if (!_new_occupied[_new_i]) {\
                        memcpy((uint8_t*)_hm_new + (_item_size * _new_i),  &(*hm)[_old_i], _item_size);\
                        _new_occupied[_new_i] = true;\
                        _new_tombstone[_new_i] = false;\
                        break;\
                    }\
\
                    if (_new_i == _new_cap)\
                        _new_i = 0;\
                    ++_new_i;\
                }\
\
            }\
        }\
\
        void *_old_ptr = *hm;\
        *hm  = _hm_new;\
        _cap = get_hm_capacity(*hm);\
        _count = get_hm_count(*hm);\
        Hash_Map_Header *_header= get_hm_header(*hm);\
        _header->occupied = _new_occupied;\
        _header->tombstone = _new_tombstone;\
        for (int i = 0; i < _header->capacity; ++i) {\
            if (_header->occupied[i]) {\
                (*hm)[i].key = strdup((*hm)[i].key);\
            }\
        }\
        void * _hold = *hm;\
        *hm =  _old_ptr;\
        cstr_hash_free(hm);\
        *hm = _hold;\
\
    }\
\
    Hash_Map_Header *_hm_header = get_hm_header(*hm);\
    unsigned int _index;\
    hash_function(item.key, strlen(item.key), _index); \
    _index %= _cap;\
    while (1) {\
        if (cstr_equals_cstr(item.key ,(*hm)[_index].key )) {\
            bool _tombstone = _hm_header->tombstone[_index];\
            (*hm)[_index] = item;\
            (*hm)[_index].key = strdup((*hm)[_index].key);\
            _hm_header->occupied[_index] = true;\
            _hm_header->tombstone[_index] = _tombstone;\
            break;\
        } else if (!_hm_header->occupied[_index]) {\
            bool _tombstone = _hm_header->tombstone[_index];\
            (*hm)[_index] = item;\
            (*hm)[_index].key = strdup((*hm)[_index].key);\
            _hm_header->occupied[_index] = true;\
            _hm_header->tombstone[_index] = _tombstone;\
            _hm_header->count +=1;\
            break;\
        }\
\
        ++_index;\
        if (_index == _cap)\
            _index = 0;\
    }\
\
\
}while(0)

#define cstr_hash_find(hm, item, return_value) do{\
    if (hm) {\
    int _cap = get_hm_capacity(hm);\
    unsigned int _index;\
    hash_function(item.key, strlen(item.key), _index);\
    _index %= _cap;\
    return_value= -1;\
    Hash_Map_Header *_hm_header = get_hm_header(hm);\
    for (int search_count = 0; search_count < _cap; ++search_count) {\
        if (_hm_header->occupied[_index]) {\
            if (!strcmp(hm[_index].key ,item.key)) {\
                return_value = _index;\
                break;\
            }\
        }else if (!_hm_header->tombstone[_index]) {\
            break;\
        }\
\
        _index += 1;\
        if (_index == _cap)\
            _index = 0;\
\
    }\
    }else {\
        return_value = -1;\
    }\
}while(0)

#define cstr_hash_remove(hm, item, return_value) do{\
    int _cap = get_hm_capacity(hm);\
    unsigned int _index;\
    hash_function(item.key, strlen(item.key),_index);\
    _index %= _cap;\
\
    return_value = false;\
    Hash_Map_Header *_hm_header = get_hm_header(hm);\
    for (int search_count = 0; search_count < _cap; ++search_count) {\
        if (_hm_header->occupied[_index]) {\
            if (!strcmp(hm[_index].key,item.key)) {\
                _hm_header->occupied[_index] = false;\
                _hm_header->tombstone[_index] = true;\
                _hm_header->count -=1;\
                return_value = true;\
                break;\
            }\
        }else if (!_hm_header->tombstone[_index]) {\
            break;\
        }\
        \
        _index += 1;\
        if (_index == _cap)\
        _index = 0;\
        \
    }\
}while(0)

#endif // _DYNAMIC_ARRAY_H_

