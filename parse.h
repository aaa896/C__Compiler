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

typedef struct Parse_Node  Parse_Node;

typedef enum {
    LINKAGE_TYPE_ERROR,
    LINKAGE_TYPE_NONE,
    LINKAGE_TYPE_EXTERNAL,
    LINKAGE_TYPE_INTERNAL,
}Linkage_Type;


typedef struct {
    char * key;
    int size;
} Type;

//typedef struct {
//    Type type;
//    Register register_type;
//    int stack_offset;
//}Param_Table_Item;


typedef enum{
    VAR_TABLE_ITEM_TYPE_STACK,
    //VAR_TABLE_ITEM_TYPE_REGISTER,
}Var_Table_Item_Type ;

typedef struct {
    char* key;
    Var_Table_Item_Type type;
    Linkage_Type linkage_type;
    union {
        struct {
            int index;
        }stack;
    };
    Type param_type;
} Var_Table_Item;


typedef struct {
    char* key;
} Label_Table_Item;



typedef struct {
    char* key;
    char* def;

} Typedef_Table_Item;



typedef struct {
    char* key;
    Type  return_type;
    Parse_Node *params;
    //statements or declarations
    Parse_Node *block_items;
    //Parse_Table var_table;
    int stack_count;
    Linkage_Type linkage_type;
}Function_Table_Item;

typedef struct  {
    //Param_Table_Item  *param_items;
    Var_Table_Item       *var_items;
    Label_Table_Item     *label_items;
    Typedef_Table_Item   *typedef_items;
    Function_Table_Item  *function_items;
    //Parse_Node *function_declarations;

    Type *valid_type_items;

    Parse_Node *continue_label;
    Parse_Node *break_label;

    Parse_Node *switch_var;
    Parse_Node **case_statements;
    Parse_Node **default_statement;
    int default_statement_index;
}Parse_Table;


typedef struct {
    Parse_Table global_table;
    Parse_Table *scope_tables;
    Token **tokens;
    int token_index;
    //int scope_id;
    //int next_scope_id;
}Parse_Data;

typedef enum Parse_Type  Parse_Type;
enum Parse_Type  {
    PARSE_TYPE_ERROR,
    PARSE_TYPE_PROGRAM,

    PARSE_TYPE_STATEMENT_GOTO,
    PARSE_TYPE_STATEMENT_LABEL,
    PARSE_TYPE_STATEMENT_DECLARATION_VAR,
    PARSE_TYPE_STATEMENT_DECLARATION_FUNCTION,
    PARSE_TYPE_STATEMENT_RETURN,
    PARSE_TYPE_STATEMENT_IF,
    PARSE_TYPE_STATEMENT_EXPRESSION,
    PARSE_TYPE_STATEMENT_NULL,
    PARSE_TYPE_STATEMENT_COMPOUND,
    PARSE_TYPE_STATEMENT_DO_WHILE,
    PARSE_TYPE_STATEMENT_WHILE,
    PARSE_TYPE_STATEMENT_FOR,
    PARSE_TYPE_STATEMENT_SWITCH,
    PARSE_TYPE_STATEMENT_CASE,
    PARSE_TYPE_STATEMENT_DEFAULT,
    PARSE_TYPE_STATEMENT_CONTINUE,
    PARSE_TYPE_STATEMENT_BREAK,
    
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
    PARSE_TYPE_EXP_FACTOR_FUNCTION_CALL,

    PARSE_TYPE_EXP_FACTOR_POST_INCREMENT,
    PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT,

    PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE,
    PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT,
    PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT,

    PARSE_TYPE_PARAMETER,
};


struct Parse_Node  {
    Parse_Type type;
    union {
        struct {
            Parse_Data data;
            //Parse_Node *function_declarations;
        }program;

        struct {
            Parse_Node *items;
        }block_items;

        struct {
            union {
                struct {
                    Parse_Node *expression;
                }return_statement;
                struct {
                    Parse_Node *expression;
                }expression_statement;
                struct {
                    Parse_Node *condition;
                    Parse_Node *then;
                    Parse_Node *else_clause;
                }if_statement;
                //TODO make goto statement have Parse_Node * to label_statement
                struct {
                    String label_identifier;
                }goto_statement;
                struct {
                    Label_Table_Item item;
                }label_statement;
                struct {
                    Parse_Node *block_items;
                }compound;
                struct {
                    Parse_Node *condition_expression;
                    Parse_Node *body_statement;
                    Parse_Node *continue_label_jump;
                    Parse_Node *break_label_jump;
                }while_statement;
                struct {
                    Parse_Node *condition_expression;
                    Parse_Node *body_statement;
                    Parse_Node *continue_label_jump;
                    Parse_Node *break_label_jump;
                }do_while_statement;
                struct {
                    //declaration or expression
                    Parse_Node *init;
                    Parse_Node *continue_label_jump;
                    Parse_Node *condition_expression;
                    Parse_Node *post_expression;
                    Parse_Node *body_statement;
                    Parse_Node *break_label_jump;
                }for_statement;
                struct {
                    Parse_Node *switch_var;
                    //Parse_Node *body_statement;

                    int default_statement_index;

                    Parse_Node *break_label_jump;
                    Parse_Node *continue_label_jump;

                    Parse_Node *case_statements;
                    Parse_Node *default_statement;
                }switch_statement;
                struct {
                    //Parse_Node *switch_var;
                    Parse_Node *number;
                    Parse_Node *body_statement;
                    Parse_Node *body_label_jump;

                    //Parse_Node *break_label_jump;
                    //Parse_Node *continue_label_jump;
                }case_statement;
                struct {
                    Parse_Node *body_statement;
                    Parse_Node *body_label_jump;
                    //Parse_Node *break_label_jump;
                    //Parse_Node *continue_label_jump;
                }default_statement;
                //struct {
                //    String label_identifier;
                //}continue_statement;
                //struct {
                //    String label_identifier;
                //}break_statement;
            };
        }statement;

        struct {
            union {
                struct {
                    Parse_Node *l_value;
                    Parse_Node *r_value;
                }var;
                struct {
                    char*  item_name;
                }function;
            };
        }declaration;

        struct {
            Type type;
            String name;
            int stack_offset;
        }parameter;



        struct {
            struct {
                union {
                    struct {
                        Parse_Node *expression;
                    }expression;

                    struct {
                        Var_Table_Item item;
                    }var;

                    struct {
                        int value;
                        String string;
                    }int_value;

                    struct {
                        Parse_Node *next;
                    }unop;

                    struct {
                        Parse_Node *value;
                    }increment;

                    //int int_value;
                    //int scope_id;
                    //String var_name;

                    struct {
                        //expressions
                        Parse_Node *args;
                        Type return_type;
                        //Type type;
                        char* item_name;
                    }function_call;
                };

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

Parse_Node *parse_tokens(Token **tokens) ;
void free_parse_nodes(Parse_Node *nodes) ;
void print_parse_nodes(Parse_Node *nodes);
#endif // _PARSE_H_


