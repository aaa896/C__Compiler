#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "string.h"

typedef enum {
    REGISTER_TYPE_EAX,
}Register_Type;

typedef struct {
    Register_Type type;
    String name;
}Register;

extern Register register_eax;

#endif // _REGISTER_H_

