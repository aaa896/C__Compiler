#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include "tac.h"
#include "string.h"

typedef enum  Assembly_Node_Type Assembly_Node_Type;
enum  Assembly_Node_Type {
    ASSEMBLY_NODE_TYPE_ERROR,
    ASSEMBLY_NODE_TYPE_PROGRAM,
    ASSEMBLY_NODE_TYPE_FUNCTION,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NEGATE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_BITWISE_NOT,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NOT,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_RET,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_ALLOCATE_STACK,

    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_ADD,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_SUB,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_IDIV,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_CDQ,

    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_AND,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_OR,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_XOR,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT,


    ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_JMP,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPNE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETNE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETG,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETGE,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETL,
    ASSEMBLY_NODE_TYPE_INSTRUCTION_SETLE,

    ASSEMBLY_NODE_TYPE_OPERAND_INT,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_EAX,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_AL,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_EDX,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_DL,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_ECX,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_CL,
    ASSEMBLY_NODE_TYPE_OPERAND_STACK,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_R10 ,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_R10B ,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_R11 ,
    ASSEMBLY_NODE_TYPE_OPERAND_REG_R11B ,
};

typedef struct Assembly_Node_Operand  Assembly_Node_Operand;
struct Assembly_Node_Operand  {
    Assembly_Node_Type type;
    String identifier;
    int int_value;
};


typedef struct Assembly_Node  Assembly_Node;
struct Assembly_Node  {
    Assembly_Node_Type type;
    union {
        struct {
            Assembly_Node *functions;
        }program;

        struct {
            String return_value;
            String name;
            Assembly_Node *instructions;
        }function;

        struct {
            Assembly_Node_Operand src ;
            Assembly_Node_Operand dest;
        }mov;

        struct {
            Assembly_Node_Operand src ;
            Assembly_Node_Operand dest;
        }binary;

        struct {
            Assembly_Node_Operand operand;
        }idiv;

        struct {
            Assembly_Node_Operand operand ;
        }unary;

        struct {
            Assembly_Node_Operand operand ;
        }stack;


        struct {
            Assembly_Node_Operand operand_a ;
            Assembly_Node_Operand operand_b ;
        }cmp;

        struct {
            Assembly_Node_Operand operand ;
        }set;
        struct {
            Assembly_Node_Operand operand ;
        }label;

        struct {
            Assembly_Node_Operand operand ;
        }jmp;


    };
};



Assembly_Node * create_code_gen( Tac_Node *tac_node) ;
void print_assembly_nodes(Assembly_Node *nodes);
void print_assembly_nodes_pad(Assembly_Node *nodes, int start_padding, int padding_increment);
#endif // _CODE_GEN_H_

