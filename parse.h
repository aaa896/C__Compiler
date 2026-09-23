#ifndef _PARSE_H_
#define _PARSE_H_

#include "string.h"
#include "lex.h"

#define PRECEDENCE_EQUAL                    1
#define PRECEDENCE_CONDITIONAL              3
#define PRECEDENCE_LOGICAL_OR               5
#define PRECEDENCE_LOGICAL_AND             10
#define PRECEDENCE_BITWISE_OR              15
#define PRECEDENCE_BITWISE_XOR             20
#define PRECEDENCE_BITWISE_AND             25
#define PRECEDENCE_NOT_EQUAL_TO            30
#define PRECEDENCE_EQUAL_TO                30
#define PRECEDENCE_GREATER_THAN            35
#define PRECEDENCE_GREATER_THAN_EQUAL_TO   35
#define PRECEDENCE_LESS_THAN               35
#define PRECEDENCE_LESS_THAN_EQUAL_TO      35
#define PRECEDENCE_BITWISE_SHIFT_RIGHT     40
#define PRECEDENCE_BITWISE_SHIFT_LEFT      40
#define PRECEDENCE_ADDITION                45
#define PRECEDENCE_SUBTRACTION             45
#define PRECEDENCE_MULTIPLY                50
#define PRECEDENCE_DIVIDE                  50
#define PRECEDENCE_MODULO                  50




typedef struct {
    String string;
    int stack_index;
    int scope_id;
    bool accessible;
    String type;
}Var_Table_Item;

typedef struct {
    String type;
    String def;
}Typedef_Item;

typedef struct  {
    Var_Table_Item *var_items;
    Var_Table_Item *label_items;
    Typedef_Item *typedef_items;
}Var_Table;

typedef enum Parse_Type  Parse_Type;
enum Parse_Type  {
    PARSE_TYPE_ERROR,
    PARSE_TYPE_PROGRAM,
    PARSE_TYPE_FUNCTION,

    PARSE_TYPE_STATEMENT_GOTO,
    PARSE_TYPE_STATEMENT_LABEL,
    PARSE_TYPE_STATEMENT_DECLARATION,
    PARSE_TYPE_STATEMENT_RETURN,
    PARSE_TYPE_STATEMENT_IF,
    PARSE_TYPE_STATEMENT_EXPRESSION,
    PARSE_TYPE_STATEMENT_NULL,
    PARSE_TYPE_STATEMENT_COMPOUND,
    
    PARSE_TYPE_EXP_BINOP_EQUAL,
    PARSE_TYPE_EXP_BINOP_SUB,
    PARSE_TYPE_EXP_BINOP_ADD,
    PARSE_TYPE_EXP_BINOP_MULTIPLY,
    PARSE_TYPE_EXP_BINOP_DIVIDE,
    PARSE_TYPE_EXP_BINOP_MODULO,
    
    PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT,
    PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT,
    PARSE_TYPE_EXP_BINOP_BITWISE_AND,
    PARSE_TYPE_EXP_BINOP_BITWISE_OR,
    PARSE_TYPE_EXP_BINOP_BITWISE_XOR,

    PARSE_TYPE_EXP_BINOP_LOGICAL_AND,
    PARSE_TYPE_EXP_BINOP_LOGICAL_OR,
    PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN,
    PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO,
    PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN,
    PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO,
    PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO,
    PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO,

    PARSE_TYPE_EXP_CONDITIONAL,

    PARSE_TYPE_EXP_FACTOR_EXP,
    PARSE_TYPE_EXP_FACTOR_INT,
    PARSE_TYPE_EXP_FACTOR_VAR,

    PARSE_TYPE_EXP_FACTOR_POST_INCREMENT,
    PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT,

    PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE,
    PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT,
    PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT,
};


typedef struct Parse_Node  Parse_Node;
struct Parse_Node  {
    Parse_Type type;
    union {
        struct {
            Parse_Node *functions;
            Var_Table global_table;
        }program;


        struct {
            String name;
            String return_value_name;
            //statements or declarations
            Parse_Node *block_items;
            Var_Table var_table;
            int stack_count;
        }function;

        struct {
            Parse_Node *items;
        }block_items;

        struct {
            Parse_Node *expression;
            struct {
                Parse_Node *condition;
                Parse_Node *then;
                Parse_Node *else_clause;
            }if_statement;
            struct {
                String label_identifier;
            }goto_statement;
            struct {
                String identifier;
            }label_statement;
            struct {
                Parse_Node *block_items;
            }compound;
        }statement;

        struct {
            Parse_Node *l_value;
            Parse_Node *r_value;
        }declaration;



        struct {
            struct {
                union {
                    Parse_Node *increment_value;
                    Parse_Node *unop_next;
                    Parse_Node *expression;
                };
                int int_value;
                int scope_id;
                String var_name;
            }factor;

            struct {
                Parse_Node *condition;
                Parse_Node *true_expression;
                Parse_Node *false_expression;
            }conditional;

            struct {
                String identifier;
                union {
                    int int_value;
                };
                Parse_Node *left;
                Parse_Node *right;
            }binop;
        }expression;

    };
};

Parse_Node *parse_tokens(Token *tokens) ;
void print_parse_nodes(Parse_Node *nodes);
#endif // _PARSE_H_

