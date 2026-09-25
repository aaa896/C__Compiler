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



#define get_array_header(array)  ((Array_Header*)((uint8_t*)((array))  - sizeof(Array_Header)))
#define get_array_count(array)   ((array) ? get_array_header(array)->count : 0)
#define get_array_tail(array)    ( array[get_array_count(array) -1])

#define create_array(array, cap)\
    do {\
        Array_Header header = {0};\
        header.capacity = cap ;\
        int item_size =  sizeof((*array)[0]);\
        int header_space = sizeof(Array_Header) ;\
        int array_size = header.capacity * item_size;\
        Array_Header *header_data = (Array_Header*)malloc(array_size + header_space);\
        if (!header_data) assert(0 && "create array malloc fail");\
        memset(header_data, 0, array_size + header_space);\
        memcpy(header_data, &header, sizeof(Array_Header));\
        (*array) = (typeof(*array))((uint8_t*)header_data + header_space);\
    } while (0);


#define array_append(array, item)\
    do {\
        if ((*array) == 0) {\
            create_array(array, ARRAY_CAPACITY_DEFAULT);\
        }\
        int header_space = sizeof(Array_Header) ;\
        Array_Header *header = (Array_Header*)((uint8_t*)((*array))  - header_space);\
        if (header->count == header->capacity) {\
            header->capacity *= 2;\
            int new_size =  header->capacity * sizeof((*array)[0]) + header_space;\
            header = (Array_Header*)realloc(header, new_size);\
            if (!header) assert(0 && "array realloc fail\n");\
            (*array) = (typeof(*array))((uint8_t*)header + header_space);\
        }\
        (*array)[header->count] = item;\
        header->count += 1;\
    }while(0);



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
        do {\
            if ((*array) == 0) {\
                ASSERT(0);\
            }\
            if (header->count == header->capacity) {\
                header->capacity *= 2;\
                int new_size =  header->capacity * sizeof((*array)[0]) + sizeof(Array_Header);\
                header = (Array_Header*)realloc(header, new_size);\
                if (!header) assert(0 && "array realloc fail\n");\
                (*(array)) = (typeof(*(array)))((uint8_t*)header + sizeof(Array_Header));\
            }\
        }while(0);\
    }\
    for (int i = count -1; i >= index; --i) {\
        (*array)[i +1] = (*array)[i];\
    }\
    header->count +=1;\
    (*array)[index] = item;\
}while(0)

#endif // _DYNAMIC_ARRAY_H_


