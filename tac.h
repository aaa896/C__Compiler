#ifndef _IR_GEN_H_
#define _IR_GEN_H_
#include "tdf.h"
#include "parse.h"
#include "string.h"


typedef enum Tac_Node_Type  Tac_Node_Type;
enum Tac_Node_Type  {
    TAC_NODE_ERROR,
    TAC_NODE_PROGRAM,
    TAC_NODE_FUNCTION,

    TAC_NODE_INSTRUCTION_COPY,
    TAC_NODE_INSTRUCTION_RETURN,
    TAC_NODE_INSTRUCTION_UNARY_NEGATE,
    TAC_NODE_INSTRUCTION_UNARY_BITWISE_NOT,
    TAC_NODE_INSTRUCTION_UNARY_LOGICAL_NOT,

    TAC_NODE_INSTRUCTION_BINARY_ADD,
    TAC_NODE_INSTRUCTION_BINARY_SUB,
    TAC_NODE_INSTRUCTION_BINARY_MULTIPLY,
    TAC_NODE_INSTRUCTION_BINARY_DIVIDE,
    TAC_NODE_INSTRUCTION_BINARY_MODULO,

    TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR,
    TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND,
    TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR,
    TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT,
    TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT,

    TAC_NODE_INSTRUCTION_BINARY_LOGICAL_OR,
    TAC_NODE_INSTRUCTION_BINARY_LOGICAL_AND,
    TAC_NODE_INSTRUCTION_BINARY_LESS_THAN,
    TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO,
    TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN,
    TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO,
    TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO,
    TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO,


    TAC_NODE_INSTRUCTION_JMP,
    TAC_NODE_INSTRUCTION_JIZ,
    TAC_NODE_INSTRUCTION_JINZ,

    TAC_NODE_INSTRUCTION_LABEL,

    //TAC_NODE_OPERAND_LABEL,
    TAC_NODE_OPERAND_INT,
    TAC_NODE_OPERAND_VAR,
};

typedef struct Tac_Node_Operand  Tac_Node_Operand;
struct Tac_Node_Operand  {
    Tac_Node_Type type;
    String identifier;
    int int_value;
};

typedef struct Tac_Node Tac_Node;
struct Tac_Node  {
    Tac_Node_Type type;

    union {
        struct {
            Tac_Node *functions;
        }program;
        struct {
            String name;
            String return_value_name;
            Tac_Node *instructions;
            int stack_size;
            bool return_statement;
        }function;

        struct {
            union {
                struct {
                    Tac_Node_Operand src;
                    Tac_Node_Operand dest;
                }unary;

                struct {
                    Tac_Node_Operand src1;
                    Tac_Node_Operand src2;
                    Tac_Node_Operand dest;
                }binary;

                struct {
                    Tac_Node_Operand condition;
                    Tac_Node_Operand label;
                }jmp;

                struct {
                    Tac_Node_Operand src;
                    Tac_Node_Operand dest;
                }copy;

                Tac_Node_Operand return_operand;
                struct {
                    Tac_Node_Operand operand;
                }label;
            };
            

        }instruction;
    };
};

Tac_Node * create_tac_nodes(Parse_Node *parse_program);
void print_tac_nodes(Tac_Node *node);

#endif // _IR_GEN_H_

