#include <stdio.h>

#include "lex.h"
#include "parse.h"
#include "dynamic_array.h"
#include "string.h"

char *invalid_var_names[] = {
    "void",
    "return",
    "goto",
    "switch",
    "case",
    "break",
    "for",
    "while",
};

char *linkage_names[] = {
    "extern",
};

static char *body_name = "body.";
static char *break_name = "break.";
static char *loop_start_name = "loop_start.";
static char *loop_end_name = "loop_end.";
static int loop_id = 0;

static String create_label(char *name, int id) {
    String str = str_create_from_cstr(name);
    str_append_int(&str, id);
    return str;
}

void parse_function_declaration(Parse_Data *data, Parse_Table *current_scope_table) ;
void parse_declaration_var(Parse_Data *data, Function_Table_Item *function_item, Parse_Node **root) ;
bool peek_declaration_var(Type *valid_type_items, Token *tokens, int token_index) ;
int peek_token(Token *tokens, int token_index, Token_Type type) ;
void parse_expression(Parse_Data *data, Parse_Node **expression_root, int min_precedence) ;
void parse_block_items(Parse_Data *data, Function_Table_Item *function_item, Parse_Node **root) ;
void free_var_table(Parse_Table *table) ;



void free_parse_nodes(Parse_Node *nodes) {
    if (!nodes)
        return;
    int nodes_count = get_array_count(nodes);
    for (int i =0; i < nodes_count; ++i) {
        Parse_Node *node = &nodes[i];
        if (node->type == PARSE_TYPE_ERROR) {
            FAIL_MSG("parse error type\n");
        }else if (node->type == PARSE_TYPE_PROGRAM) {
            Hash_Map_Header *functions = get_hm_header(node->program.data.global_table.function_items);
            for (int i = 0; i < functions->capacity; ++i) {
                if (functions->occupied[i]) {
                    if (node->program.data.global_table.function_items[i].params) {
                        int param_count = get_array_count(node->program.data.global_table.function_items[i].params);
                        for (int param_i = 0; param_i < param_count; ++param_i) {

                            free_parse_nodes(&node->program.data.global_table.function_items[i].params[param_i]);
                        }
                        array_free(&node->program.data.global_table.function_items[i].params);
                    }
                    if (node->program.data.global_table.function_items[i].block_items){
                        int block_item_count = get_array_count(node->program.data.global_table.function_items[i].block_items);
                        for (int block_i = 0; block_i < block_item_count; ++block_i){
                          free_parse_nodes(&node->program.data.global_table.function_items[i].block_items[block_i]);
                        }
                        array_free(&node->program.data.global_table.function_items[i].block_items);
                    }
                }
            }

            cstr_hash_free(&node->program.data.global_table.function_items);

            cstr_hash_free(&nodes->program.data.global_table.valid_type_items);

            array_free(&nodes);
        }else if (node->type == PARSE_TYPE_STATEMENT_DECLARATION_FUNCTION) {

        }else if (node->type == PARSE_TYPE_STATEMENT_COMPOUND) {
            int block_item_count = get_array_count(node->statement.compound.block_items);
            for (int i = 0; i  < block_item_count; ++i) {
                free_parse_nodes(&node->statement.compound.block_items[i]);
            }
            array_free(&node->statement.compound.block_items);
        }else if (node->type == PARSE_TYPE_STATEMENT_RETURN)      {
            free_parse_nodes(node->statement.return_statement.expression);
            array_free(&node->statement.return_statement.expression);
        }else if (node->type == PARSE_TYPE_STATEMENT_NULL)      {
        }else if (node->type == PARSE_TYPE_STATEMENT_EXPRESSION)      {
            free_parse_nodes(node->statement.expression_statement.expression);
            array_free(&node->statement.expression_statement.expression);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_INT) {
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_VAR) {
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT) {
            free_parse_nodes(node->expression.factor.increment.value);
            array_free(&node->expression.factor.increment.value);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_POST_INCREMENT) {
            free_parse_nodes(node->expression.factor.increment.value);
            array_free(&node->expression.factor.increment.value);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE) {
            free_parse_nodes(node->expression.factor.unop.next);
            array_free(&node->expression.factor.unop.next);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_EXP) {
            free_parse_nodes(node->expression.factor.expression.expression);
            array_free(&node->expression.factor.expression.expression);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT) {
            free_parse_nodes(node->expression.factor.unop.next);
            array_free(&node->expression.factor.unop.next);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT) {
            free_parse_nodes(node->expression.factor.unop.next);
            array_free(&node->expression.factor.unop.next);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_ADD) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_SUB) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_MULTIPLY) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_DIVIDE) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_MODULO) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_OR) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_AND) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_XOR) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_AND) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);


        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);

        }else if (node->type == PARSE_TYPE_STATEMENT_DECLARATION_VAR) {
            free_parse_nodes(node->declaration.var.l_value);
            array_free(&node->declaration.var.l_value);
            if (node->declaration.var.r_value) {
                free_parse_nodes(node->declaration.var.r_value);
                array_free(&node->declaration.var.r_value);
            }
        }else if (node->type == PARSE_TYPE_EXP_BINOP_EQUAL) {
            free_parse_nodes(node->expression.binop.left);
            array_free(&node->expression.binop.left);
            free_parse_nodes(node->expression.binop.right);
            array_free(&node->expression.binop.right);
        }else if (node->type == PARSE_TYPE_STATEMENT_IF) {
            free_parse_nodes(node->statement.if_statement.condition);

            array_free(&node->statement.if_statement.condition);

            free_parse_nodes(node->statement.if_statement.then);
            array_free(&node->statement.if_statement.then);

            if (node->statement.if_statement.else_clause) {
                free_parse_nodes(node->statement.if_statement.else_clause);
                array_free(&node->statement.if_statement.else_clause);

            }
        }else if (node->type == PARSE_TYPE_STATEMENT_DO_WHILE) {
            free_parse_nodes(node->statement.do_while_statement.continue_label_jump);
            array_free(&node->statement.do_while_statement.continue_label_jump);
            free_parse_nodes(node->statement.do_while_statement.break_label_jump);
            array_free(&node->statement.do_while_statement.break_label_jump);
            free_parse_nodes(node->statement.do_while_statement.body_statement);
            array_free(&node->statement.do_while_statement.body_statement);
            free_parse_nodes(node->statement.do_while_statement.condition_expression);
            array_free(&node->statement.do_while_statement.condition_expression);

        }else if (node->type == PARSE_TYPE_STATEMENT_FOR) {
            free_parse_nodes(node->statement.for_statement.init);
            array_free(&node->statement.for_statement.init);
            free_parse_nodes(node->statement.for_statement.condition_expression);
            array_free(&node->statement.for_statement.condition_expression);
            free_parse_nodes(node->statement.for_statement.body_statement);
            array_free(&node->statement.for_statement.body_statement);
            free_parse_nodes(node->statement.for_statement.post_expression);
            array_free(&node->statement.for_statement.post_expression);
            free_parse_nodes(node->statement.for_statement.continue_label_jump);
            array_free(&node->statement.for_statement.continue_label_jump);
            free_parse_nodes(node->statement.for_statement.break_label_jump);
            array_free(&node->statement.for_statement.break_label_jump);

        }else if (node->type == PARSE_TYPE_STATEMENT_WHILE) {
            free_parse_nodes(node->statement.while_statement.continue_label_jump);
            array_free(&node->statement.while_statement.continue_label_jump);
            free_parse_nodes(node->statement.while_statement.break_label_jump);
            array_free(&node->statement.while_statement.break_label_jump);
            free_parse_nodes(node->statement.while_statement.condition_expression);
            array_free(&node->statement.while_statement.condition_expression);
            free_parse_nodes(node->statement.while_statement.body_statement);
            array_free(&node->statement.while_statement.body_statement);
        }else if (node->type == PARSE_TYPE_STATEMENT_GOTO) {
        }else if (node->type == PARSE_TYPE_STATEMENT_LABEL) {
        }else if (node->type == PARSE_TYPE_EXP_CONDITIONAL) {
            free_parse_nodes(node->expression.conditional.condition);
            array_free(&node->expression.conditional.condition);
            free_parse_nodes(node->expression.conditional.true_expression);
            array_free(&node->expression.conditional.true_expression);
            free_parse_nodes(node->expression.conditional.false_expression);
            array_free(&node->expression.conditional.false_expression);
        }else if (node->type == PARSE_TYPE_STATEMENT_SWITCH) {
            free_parse_nodes(node->statement.switch_statement.break_label_jump);
            array_free(&node->statement.switch_statement.break_label_jump);
            free_parse_nodes(node->statement.switch_statement.continue_label_jump);
            array_free(&node->statement.switch_statement.continue_label_jump);
            free_parse_nodes(node->statement.switch_statement.switch_var);
            array_free(&node->statement.switch_statement.switch_var);
            int case_statement_count = get_array_count(node->statement.switch_statement.case_statements);
            for (int i = 0; i < case_statement_count; ++i) {
                free_parse_nodes(&node->statement.switch_statement.case_statements[i]);
            }
            array_free(&node->statement.switch_statement.case_statements);
            int default_statement_count = get_array_count(node->statement.switch_statement.default_statement);
            if (default_statement_count) {
                free_parse_nodes(node->statement.switch_statement.default_statement);
                array_free(&node->statement.switch_statement.default_statement);
            }
        }else if (node->type == PARSE_TYPE_STATEMENT_CASE) {
            free_parse_nodes(node->statement.case_statement.number);
            array_free(&node->statement.case_statement.number);
            free_parse_nodes(node->statement.case_statement.body_label_jump);
            array_free(&node->statement.case_statement.body_label_jump);
            int case_body_count = get_array_count(node->statement.case_statement.body_statement);
            for (int i = 0; i < case_body_count; ++i) {
                free_parse_nodes(&node->statement.case_statement.body_statement[i]);
            }
            array_free(&node->statement.case_statement.body_statement);

        }else if (node->type == PARSE_TYPE_STATEMENT_DEFAULT) {
            free_parse_nodes(node->statement.default_statement.body_label_jump);
            array_free(&node->statement.default_statement.body_label_jump);
            free_parse_nodes(node->statement.default_statement.body_statement);
            array_free(&node->statement.default_statement.body_statement);
        }else if (node->type == PARSE_TYPE_EXP_FACTOR_FUNCTION_CALL) {
            int arg_count = get_array_count(node->expression.factor.function_call.args);
            for (int i = 0; i < arg_count; ++i) {
                free_parse_nodes(&node->expression.factor.function_call.args[i]);
            }

            array_free(&node->expression.factor.function_call.args);
        }else if (node->type == PARSE_TYPE_PARAMETER) {
            //str_free(&node->parameter.name);
            //free(node->parameter.type.key);
        }else {
            FAIL_MSG("unknown free  \n");
        }

    }

}

void print_char_n(char c, int n) 
{
    for (int i =0 ; i < n; ++i) 
        putchar(c);
}


void print_parse_nodes_2(Parse_Node *node, int start_padding, int padding_increment) 
{
    if (!node)
        return;
    int node_count = get_array_count(node);
    if (node->type == PARSE_TYPE_ERROR) {
        FAIL_MSG("parse error type\n");
    }else if (node->type == PARSE_TYPE_PROGRAM) {
        printf("Program {\n");


        Hash_Map_Header *hm_header = get_hm_header(node->program.data.global_table.function_items);

        for (int i = 0; i < hm_header->capacity; ++i) {
            if(hm_header->occupied[i]) {
                start_padding += padding_increment;
                print_char_n(' ', start_padding);
                printf("FN rv: ");
                printf("%s",node->program.data.global_table.function_items[i].return_type.key);
                printf(" name:");
                printf("%s",node->program.data.global_table.function_items[i].key);
                printf("\n");
                print_char_n(' ', start_padding);
                printf("params:");
                int param_count = get_array_count(node->program.data.global_table.function_items[i].params);
                for (int param_i = 0; param_i < param_count ; ++param_i) {
                    printf(" ");
                    printf(node->program.data.global_table.function_items[i].params[param_i].parameter.type.key);
                    printf(" ");
                    str_print(node->program.data.global_table.function_items[i].params[param_i].parameter.name);
                    printf(", ");
                }
                printf("{\n");
                if (node->program.data.global_table.function_items[i].block_items) {
                    int block_item_count = get_array_count(node->program.data.global_table.function_items[i].block_items);
                    for (int block_i = 0; block_i  < block_item_count; ++block_i) {
                        print_parse_nodes_2(&node->program.data.global_table.function_items[i].block_items[block_i], start_padding + padding_increment, padding_increment);
                    }
                }
                printf("}\n");

                start_padding -= padding_increment;
            }
        }



        printf("}\n");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_FUNCTION_CALL) {
        print_char_n(' ', start_padding);
        printf("Fn Call { ");
        printf(" name:");
        printf("%s",node->expression.factor.function_call.item_name);
        printf("\n");
        print_char_n(' ', start_padding);
        printf("params:\n");
        int param_count = get_array_count(node->expression.factor.function_call.args);
        for (int i = 0; i < param_count ; ++i) {
            //printf("%s,",&node->expression.factor.function_call.type[i].key);
            print_parse_nodes_2(&node->expression.factor.function_call.args[i], start_padding + padding_increment, padding_increment);
        }
        print_char_n(' ', start_padding);
        printf("}\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_DECLARATION_FUNCTION) {
        ASSERT(0);
        //print_char_n(' ', start_padding);
        ////printf("FN rv:" SV_FMT " name:"SV_FMT" {\n", SV_ARG(node->function.return_value_name), SV_ARG(node->function.name));
        //printf("FN rv: ");
        //printf("%s",node->declaration.function.return_type.key);
        //printf(" name:");
        //printf("%s",node->declaration.function.key);
        //printf("\n");
        //print_char_n(' ', start_padding);
        //printf("params:");
        //int param_count = get_array_count(node->declaration.function.params);
        //for (int i = 0; i < param_count ; ++i) {
        //    printf(" ");
        //    printf(node->declaration.function.params[i].parameter.type.key);
        //    printf(" ");
        //    str_print(node->declaration.function.params[i].parameter.name);
        //    printf(", ");
        //}
        //printf("{\n");
        //if (node->declaration.function.block_items) {
        //    int block_item_count = get_array_count(node->declaration.function.block_items);
        //    for (int i = 0; i  < block_item_count; ++i) {
        //        print_parse_nodes_2(&node->declaration.function.block_items[i], start_padding + padding_increment, padding_increment);
        //    }
        //}
        //print_char_n(' ', start_padding);
        printf("}\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_COMPOUND) {
        print_char_n(' ', start_padding);
        printf("Compound Statement {\n");
        int block_item_count = get_array_count(node->statement.compound.block_items);
        for (int i = 0; i  < block_item_count; ++i) {
            print_parse_nodes_2(&node->statement.compound.block_items[i], start_padding + padding_increment, padding_increment);
        }
        print_char_n(' ', start_padding);
        printf("}\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_RETURN)      {
        print_char_n(' ', start_padding);
        printf("Statement: Return (\n");
        print_parse_nodes_2(node->statement.return_statement.expression, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_STATEMENT_NULL)      {
        print_char_n(' ', start_padding);
        printf("Statement: Null; \n");
    }else if (node->type == PARSE_TYPE_STATEMENT_EXPRESSION)      {
        print_char_n(' ', start_padding);
        printf("Statement: Expression (\n");
        print_parse_nodes_2(node->statement.expression_statement.expression, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_FACTOR_INT) {
        print_char_n(' ', start_padding);
        printf("Int %d \n", node->expression.factor.int_value.value);
        printf("\n");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_VAR) {
        print_char_n(' ', start_padding);
        printf("Var :");
        printf("%s",node->expression.factor.var.item.key);
        printf(" stack:%d\n",   node->expression.factor.var.item.stack.index);

    }else if (node->type == PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT) {
        print_char_n(' ', start_padding);
        printf("Pre Increment ++(\n");
        print_parse_nodes_2(node->expression.factor.increment.value, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_POST_INCREMENT) {
        print_char_n(' ', start_padding);
        printf("Post Increment ++(\n");
        print_parse_nodes_2(node->expression.factor.increment.value, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE) {
        print_char_n(' ', start_padding);
        printf("Unop Negate -(\n");
        print_parse_nodes_2(node->expression.factor.unop.next, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_EXP) {

        print_parse_nodes_2(node->expression.factor.expression.expression, start_padding, padding_increment);
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT) {
        print_char_n(' ', start_padding);
        printf("Unop Bitwise ~(\n ");
        print_parse_nodes_2(node->expression.factor.unop.next, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT) {
        print_char_n(' ', start_padding);
        printf("Unop Logical Not !(\n ");
        print_parse_nodes_2(node->expression.factor.unop.next, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_ADD) {
        print_char_n(' ', start_padding);
        printf("Binop Add + (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_SUB) {
        print_char_n(' ', start_padding);
        printf("Binop Sub - (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_MULTIPLY) {
        print_char_n(' ', start_padding);
        printf("Binop Mult * (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_DIVIDE) {
        print_char_n(' ', start_padding);
        printf("Binop Divide / (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_MODULO) {
        print_char_n(' ', start_padding);
        printf("Binop Modulo %% (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_OR) {
        print_char_n(' ', start_padding);
        printf("Binop Bitwise Or | (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_AND) {
        print_char_n(' ', start_padding);
        printf("Binop Bitwise And & (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_XOR) {
        print_char_n(' ', start_padding);
        printf("Binop Bitwise Xor ^ (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT) {
        print_char_n(' ', start_padding);
        printf("Binop Bitwise Shift Left << (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT) {
        print_char_n(' ', start_padding);
        printf("Binop Shift Right >> (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_AND) {
        print_char_n(' ', start_padding);
        printf("Binop Logical And && (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Or || (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Less Than < (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Less Than Equal to <= (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Greater Than > (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Greater Than Equal to >= (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");


    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Not Equal to != (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO) {
        print_char_n(' ', start_padding);
        printf("Binop Logical Equal to == (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_STATEMENT_DECLARATION_VAR) {
        print_char_n(' ', start_padding);
        printf("Declaration  (\n ");
        print_parse_nodes_2(node->declaration.var.l_value,  start_padding + padding_increment, padding_increment);
        if (node->declaration.var.r_value) {
            print_parse_nodes_2(node->declaration.var.r_value,  start_padding + padding_increment, padding_increment);
        }
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_BINOP_EQUAL) {
        print_char_n(' ', start_padding);
        printf("Binop Equal = (\n ");
        print_parse_nodes_2(node->expression.binop.left,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->expression.binop.right, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_STATEMENT_IF) {
        print_char_n(' ', start_padding);
        printf("If  (\n ");
        print_char_n(' ', start_padding);
        printf("condition (\n");
        print_parse_nodes_2(node->statement.if_statement.condition,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");
        print_char_n(' ', start_padding);
        printf("then (\n");
        print_parse_nodes_2(node->statement.if_statement.then,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");

        if (node->statement.if_statement.else_clause) {
            print_char_n(' ', start_padding);
            printf("else (\n ");
            print_parse_nodes_2(node->statement.if_statement.else_clause,  start_padding + padding_increment, padding_increment);
            print_char_n(' ', start_padding);
            printf(")\n");

        }
        print_char_n(' ', start_padding);
    }else if (node->type == PARSE_TYPE_STATEMENT_DO_WHILE) {
        //print_parse_nodes_2(node->statement.while_statement.continue_label_jump,  padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf("Do  While ");
        printf("%s",node->statement.do_while_statement.continue_label_jump->statement.label_statement.item.key);
        printf(" (\n ");
        print_char_n(' ', start_padding);
        printf("Body (\n");
        print_parse_nodes_2(node->statement.do_while_statement.body_statement,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(") ");
        printf("\n");
        //print_parse_nodes_2(node->statement.while_statement.break_label_jump,   padding_increment, padding_increment);


        print_char_n(' ', start_padding);
        printf("condition (\n");
        print_parse_nodes_2(node->statement.do_while_statement.condition_expression,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");

        print_char_n(' ', start_padding);
        printf(")");
        printf("%s", node->statement.do_while_statement.break_label_jump->statement.label_statement.item.key);
        printf("\n");

    }else if (node->type == PARSE_TYPE_STATEMENT_FOR) {
        //print_parse_nodes_2(node->statement.while_statement.continue_label_jump,  padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf("For ");
        printf("%s",node->statement.for_statement.continue_label_jump->statement.label_statement.item.key);
        printf(" (\n ");

        print_char_n(' ', start_padding);
        printf("Init: (\n");
        print_parse_nodes_2(node->statement.for_statement.init,  start_padding + padding_increment, padding_increment);
        printf("\n ");
        print_char_n(' ', start_padding);
        printf(")\n");

        print_char_n(' ', start_padding);
        printf("Condition: (\n");
        print_char_n(' ', start_padding);
        print_parse_nodes_2(node->statement.for_statement.condition_expression,  start_padding + padding_increment, padding_increment);
        printf("\n ");
        print_char_n(' ', start_padding);
        printf(")\n");

        print_char_n(' ', start_padding);
        printf("Body: (\n");
        print_char_n(' ', start_padding);
        print_parse_nodes_2(node->statement.for_statement.body_statement,  start_padding + padding_increment, padding_increment);
        printf("\n ");
        print_char_n(' ', start_padding);
        printf(")\n");

        printf("\n");
        print_char_n(' ', start_padding);
        printf(")");
        printf("%s",node->statement.for_statement.break_label_jump->statement.label_statement.item.key);
        printf("\n");

    }else if (node->type == PARSE_TYPE_STATEMENT_WHILE) {
        //print_parse_nodes_2(node->statement.while_statement.continue_label_jump,  padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf("While ");
        printf(node->statement.while_statement.continue_label_jump->statement.label_statement.item.key);
        printf(" (\n ");
        print_char_n(' ', start_padding);
        printf("condition (\n");
        print_parse_nodes_2(node->statement.while_statement.condition_expression,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");
        print_char_n(' ', start_padding);
        printf("Body (\n");
        print_parse_nodes_2(node->statement.while_statement.body_statement,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(") ");
        printf("\n");
        //print_parse_nodes_2(node->statement.while_statement.break_label_jump,   padding_increment, padding_increment);


        print_char_n(' ', start_padding);
        printf(")");
        printf("%s", node->statement.while_statement.break_label_jump->statement.label_statement.item.key);
        printf("\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_GOTO) {
        print_char_n(' ', start_padding);
        printf("Goto  (\n ");
        print_char_n(' ', start_padding);
        str_print(node->statement.goto_statement.label_identifier);
        printf("\n");
        print_char_n(' ', start_padding);
        printf(")\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_LABEL) {
        print_char_n(' ', start_padding);
        printf("Label  (\n ");
        print_char_n(' ', start_padding);
        printf("%s",node->statement.label_statement.item.key);
        printf("\n");
        print_char_n(' ', start_padding);
        printf(")\n");

    }else if (node->type == PARSE_TYPE_EXP_CONDITIONAL) {
        print_char_n(' ', start_padding);
        printf("Conditional  (\n ");
        print_char_n(' ', start_padding);
        printf("condition (\n");
        print_parse_nodes_2(node->expression.conditional.condition,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");
        print_char_n(' ', start_padding);
        printf("true (\n");
        print_parse_nodes_2(node->expression.conditional.true_expression,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");

        print_char_n(' ', start_padding);
        printf("false (\n");
        print_parse_nodes_2(node->expression.conditional.false_expression,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(")\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_SWITCH) {
        print_char_n(' ', start_padding);
        printf("Switch  ( \n");
        print_parse_nodes_2(node->statement.switch_statement.break_label_jump,  start_padding + padding_increment, padding_increment);
        if (node->statement.switch_statement.continue_label_jump)
            print_parse_nodes_2(node->statement.switch_statement.continue_label_jump,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->statement.switch_statement.switch_var,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" ) {\n");
        int case_statement_count = get_array_count(node->statement.switch_statement.case_statements);
        for (int i = 0; i < case_statement_count; ++i) {
            print_parse_nodes_2(&node->statement.switch_statement.case_statements[i],  start_padding + padding_increment, padding_increment);
        }
        int default_statement_count = get_array_count(node->statement.switch_statement.default_statement);
        ASSERT(default_statement_count <= 1);
        if (default_statement_count)
            print_parse_nodes_2(node->statement.switch_statement.default_statement,  start_padding + padding_increment, padding_increment);
        //print_parse_nodes_2(node->statement.switch_statement.body_statement,  start_padding + padding_increment, padding_increment);
        printf("\n");
        print_char_n(' ', start_padding);
        printf("}\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_CASE) {
        print_char_n(' ', start_padding);
        printf("Case  (\n");
        print_parse_nodes_2(node->statement.case_statement.number,  start_padding + padding_increment, padding_increment);
        print_parse_nodes_2(node->statement.case_statement.body_label_jump,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" ) {\n");
        int case_body_count = get_array_count(node->statement.case_statement.body_statement);
        for (int i = 0; i < case_body_count; ++i) {
            print_parse_nodes_2(&node->statement.case_statement.body_statement[i],  start_padding + padding_increment, padding_increment);
        }
        print_char_n(' ', start_padding);
        printf("}\n");

    }else if (node->type == PARSE_TYPE_STATEMENT_DEFAULT) {
        print_char_n(' ', start_padding);
        printf("Default  (\n");
        print_parse_nodes_2(node->statement.default_statement.body_label_jump,  start_padding + padding_increment, padding_increment);
        //print_parse_nodes_2(node->statement.default_statement.break_label_jump,  start_padding + padding_increment, padding_increment);
        //print_parse_nodes_2(node->statement.default_statement.continue_label_jump,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" ) {\n");
        //print_parse_nodes_2(node->statement.case_statement.number,  start_padding + padding_increment, padding_increment);
        //print_char_n(' ', start_padding);
        //printf(" ) {\n");
        print_parse_nodes_2(node->statement.default_statement.body_statement,  start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf("}\n");
    }else {
        FAIL_MSG("unknown parse  print\n");
    }

}


void print_parse_nodes(Parse_Node *nodes)
{
    printf("\nParse Nodes\n");
    int padding_increment = 4;
    int start_padding = 0;
    print_parse_nodes_2(nodes, start_padding, padding_increment);
}

//void increase_scope_id(int *scope_id, int *next_scope_id) {
//*scope_id = *next_scope_id;
//*next_scope_id += 1;
//}

b32 peek_parse_typedef(Parse_Table *table, Token *tokens, int *token_index) {
    if (peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER) && str_equals_cstr(tokens[*token_index].identifier, "typedef")) {
        *token_index +=1;
        Typedef_Table_Item item = {0};
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER))
            ASSERT(0);
        item.key = str_convert_to_cstr(tokens[*token_index].identifier);
        *token_index +=1;
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER))
            ASSERT(0);
        item.def = str_convert_to_cstr(tokens[*token_index].identifier);
        *token_index +=1;
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON))
            ASSERT(0);
        *token_index +=1;

        int hash_index = -1;
        cstr_hash_find(table->typedef_items, item.key,  hash_index);
        if (hash_index != -1)
            ASSERT(0);
        cstr_hash_add(&table->typedef_items, item);

        return true;
    }
    return false;
}

//int get_var_table_index(Parse_Table *var_table, String identifier, int scope_id) {
//    if (var_table->var_items) {
//        int var_table_count = get_array_count(var_table->var_items);
//        for (int i = 0; i < var_table_count; ++i) {
//            if (var_table->var_items[i].accessible) {
//                if (var_table->var_items[i].scope_id == scope_id) {
//                    String table_string = var_table->var_items[i].string;
//                    if (str_equals_str(table_string, identifier)) {
//                        return i;
//                    }
//                }
//            }
//        }
//
//        for (int i = var_table_count - 1; i >= 0; --i) {
//            if (var_table->var_items[i].accessible) {
//                String table_string = var_table->var_items[i].string;
//                if (str_equals_str(table_string, identifier)) {
//                    return i;
//                }
//            }
//        }
//
//    }
//    return -1;
//}


//void insert_label_table_item(Label_Table_Item **labels, String identifier) {
//    Label_Table_Item item = {0};
//    item.string = str_clone(identifier);
//    array_append(labels, item);
//}

//void insert_var_table_item(Var_Table_Item **items, String identifier, int scope_id, int *stack_count) {
//    Var_Table_Item item = {0};
//    item.key = str_convert_to_cstr(identifier);
//    item.scope_id = scope_id;
//    item.stack_index = *stack_count;
//    //item.accessible =true;
//    (*stack_count) += 1;
//    array_append(items, item);
//}

void free_var_table(Parse_Table *table) {
    if (table->var_items)
        cstr_hash_free(&table->var_items);
    if (table->label_items)
        cstr_hash_free(&table->label_items);
    if (table->typedef_items)
        cstr_hash_free(&table->typedef_items);
    if (table->function_items)
        array_free(&table->function_items);
    if (table->valid_type_items)
        cstr_hash_free(&table->valid_type_items);
}



int peek_token(Token *tokens, int token_index, Token_Type type) 
{
    if ((int)get_array_count(tokens) <= token_index)
        return 0;
    return (tokens[token_index].type == type);
}

bool expression_type_matches(Parse_Node *expression, char *type) {
    if (expression->type == PARSE_TYPE_EXP_FACTOR_VAR) {
        if (!cstr_equals_cstr(expression->expression.factor.var.item.param_type.key, type)) {
            return false;
        }
        return true;

    } else  if (expression->type > PARSE_TYPE_EXP_FACTOR_UNOP_BEGIN && 
            expression->type  < PARSE_TYPE_EXP_FACTOR_UNOP_END) {
        bool result = expression_type_matches(expression->expression.factor.unop.next, type);
        return result;
    } else  if (expression->type == PARSE_TYPE_EXP_FACTOR_INT) {
        if (!cstr_equals_cstr("int", type)) {
            return false;
        }
        return true;

    } else  if (expression->type > PARSE_TYPE_EXP_BINOP_START &&
            expression->type < PARSE_TYPE_EXP_BINOP_END) {
        bool left_match = expression_type_matches(expression->expression.binop.left, type);
        bool right_match = expression_type_matches(expression->expression.binop.right, type);
        return (left_match && right_match);
    } else {
        return false;
    }
}


void parse_factor(Parse_Data *data, Parse_Node **factor_root)
{
    if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_NUMBER)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_INT;
        factor.expression.factor.int_value.value = (*data->tokens)[data->token_index].constant;
        factor.expression.factor.int_value.string = str_clone((*data->tokens)[data->token_index].identifier);
        array_append(factor_root, factor);
        (data->token_index)++;

    }else if (peek_token((*data->tokens), data->token_index, TOKEN_TYPE_NEGATE)) {

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE;
        (data->token_index)++;
        parse_factor(data, &(factor.expression.factor.unop.next));
        array_append(factor_root, factor);

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER)) {
        bool is_function_call = peek_token(*data->tokens, data->token_index + 1, TOKEN_TYPE_OPEN_PAREN);
        if (is_function_call) {
            Parse_Node function_call = {0};
            function_call.type = PARSE_TYPE_EXP_FACTOR_FUNCTION_CALL;
            char* function_name = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
            function_call.expression.factor.function_call.item_name = function_name;
            data->token_index += 2;
            int hash_index ;

            Function_Table_Item *function;
            {
                int current_scope = get_array_count(data->scope_tables) -1;
                Parse_Table *current_scope_table = &data->scope_tables[current_scope];
                cstr_hash_find(current_scope_table->var_items, function_name, hash_index);
                ASSERT(hash_index == -1);
                while (current_scope >= 0) {
                    Parse_Table *current_scope_table = &data->scope_tables[current_scope];
                    cstr_hash_find(current_scope_table->function_items, function_name, hash_index);
                    if (hash_index != -1) {
                        function = &data->scope_tables[current_scope].function_items[hash_index];
                        break;
                    }
                    --current_scope;
                }

            }
            if (hash_index == -1) {
                cstr_hash_find(data->global_table.function_items, function_name, hash_index);
                function = &data->global_table.function_items[hash_index];
            }
            ASSERT(hash_index != -1);
            int current_scope = get_array_count(data->scope_tables) -1;
            Parse_Table *current_scope_table = &data->scope_tables[current_scope];
            cstr_hash_find(current_scope_table->var_items, function_name, hash_index);
            ASSERT(hash_index == -1);

            int function_param_count = get_array_count(function->params);
            if (!function_param_count) {
                data->token_index +=1;
            }
            for (int i = 0; i < function_param_count ; ++i) {
                ASSERT(!peek_token(*data->tokens, data->token_index , TOKEN_TYPE_CLOSE_PAREN));
                Parse_Node *function_param = &function->params[i];
                Parse_Node *call_param  = 0;
                parse_expression(data, &call_param, 0);
                ASSERT(expression_type_matches(call_param, function_param->parameter.type.key));
                

                if (!peek_token(*data->tokens, data->token_index , TOKEN_TYPE_COMMA)) {
                    ASSERT(i +1 == function_param_count);
                    ASSERT(peek_token(*data->tokens, data->token_index , TOKEN_TYPE_CLOSE_PAREN));
                } 
                array_append(&function_call.expression.factor.function_call.args, *call_param);
                array_free(&call_param);

                data->token_index += 1;
            }
            array_append(factor_root, function_call);
        }else {
            bool post_increment = peek_token(*data->tokens, data->token_index + 1, TOKEN_TYPE_INCREMENT);
            Parse_Node factor = ZERO_STRUCT;
            factor.type = PARSE_TYPE_EXP_FACTOR_VAR;
            factor.expression.factor.var.item.key =  str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
            int var_index = -1;
            int scope_index = get_array_count(data->scope_tables) -1;
            cstr_hash_find((data->scope_tables[scope_index].var_items),  factor.expression.factor.var.item.key , var_index);
            while (var_index == -1 && scope_index > 0) {
                scope_index -=1;
                cstr_hash_find((data->scope_tables[scope_index].var_items),  factor.expression.factor.var.item.key , var_index);
            }

            //int var_table_index = get_var_table_index(&data->function_table, factor.expression.factor.var.name, data->scope_id);
            ASSERT( var_index != -1);
            char * name =  factor.expression.factor.var.item.key;
            factor.expression.factor.var.item = data->scope_tables[scope_index].var_items[var_index];
            factor.expression.factor.var.item.key = name;
            //    factor.expression.factor.var.item = data->scope_tables[scope_index].var_items[var_index];
            //factor.expression.factor.var.item.scope_id     = data->function_table.var_items[var_index].scope_id;

            if (post_increment) {
                Parse_Node post = {0};
                post.type = PARSE_TYPE_EXP_FACTOR_POST_INCREMENT;
                array_append(&post.expression.factor.increment.value, factor);
                array_append(factor_root, post);
                (data->token_index)++;
            } else {
                data->token_index +=1;
            }

            array_append(factor_root, factor);
        }

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_INCREMENT)) {


        ASSERT(data->token_index < get_array_count(*data->tokens) -1);
        char* var_str = str_convert_to_cstr((*data->tokens)[data->token_index + 1].identifier);



        int var_index = -1;
        int scope_index = get_array_count(data->scope_tables) -1;

        cstr_hash_find(data->scope_tables[scope_index].var_items,var_str, var_index);
        while (var_index == -1 && scope_index > 0) {
            scope_index -=1;
            cstr_hash_find(data->scope_tables[scope_index].var_items, var_str, var_index);
        }

        ASSERT( var_index != -1);

        int var_stack_index = data->scope_tables[scope_index].var_items[var_index].stack.index;

        (data->token_index) +=1;

        Parse_Node pre = {0};
        pre.type = PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT;

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_VAR;
        factor.expression.factor.var.item.key         = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
        factor.expression.factor.var.item.stack.index  = var_stack_index;

        array_append(&pre.expression.factor.increment.value, factor );
        array_append(factor_root, pre);
        (data->token_index) +=1;
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_TILDE)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT;
        (data->token_index)++;
        parse_factor(data, &(factor.expression.factor.unop.next));
        array_append(factor_root, factor);
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_EXCLAMATION)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT;
        (data->token_index)++;
        parse_factor(data, &(factor.expression.factor.unop.next));
        array_append(factor_root, factor);
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_PAREN)) {

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_EXP;
        (data->token_index)++;
        parse_expression(data, &(factor.expression.factor.expression.expression),  0);
        if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_PAREN)) {
            FAIL_MSG("expected exp number type\n");
        }
        (data->token_index)++;
        array_append(factor_root, factor);
    }else {
        FAIL_MSG(" Uknown factor\n");
    }

}



Parse_Type peek_binop_type(Token **tokens, int token_index) 
{
    Parse_Type binop_type = PARSE_TYPE_ERROR;


    if (peek_token(*tokens, token_index, TOKEN_TYPE_PLUS_EQUAL)                ||
            peek_token(*tokens, token_index, TOKEN_TYPE_ASTERIX_EQUAL)         ||
            peek_token(*tokens, token_index, TOKEN_TYPE_FORWARD_SLASH_EQUAL)   ||
            peek_token(*tokens, token_index, TOKEN_TYPE_PERCENT_EQUAL)         ||
            peek_token(*tokens, token_index, TOKEN_TYPE_AMPERSAND_EQUAL)       ||
            peek_token(*tokens, token_index,   TOKEN_TYPE_VERTICAL_BAR_EQUAL ) ||
            peek_token(*tokens, token_index,TOKEN_TYPE_CARROT_EQUAL)           ||
            peek_token(*tokens, token_index,TOKEN_TYPE_LEFT_SHIFT_EQUAL)       ||
            peek_token(*tokens, token_index,TOKEN_TYPE_RIGHT_SHIFT_EQUAL) )
    {
        Token binop = {0};
        ASSERT(token_index >= 1);
        Token identifier = (*tokens)[token_index -1];
        identifier.identifier = str_clone(identifier.identifier);
        ASSERT(identifier.type == TOKEN_TYPE_IDENTIFIER);
        array_insert(tokens,token_index + 1, identifier);
        if (peek_token((*tokens), token_index, TOKEN_TYPE_PLUS_EQUAL)) {
            binop.type = TOKEN_TYPE_PLUS;
        } else if (peek_token((*tokens), token_index, TOKEN_TYPE_ASTERIX_EQUAL)) {
            binop.type = TOKEN_TYPE_ASTERIX;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_FORWARD_SLASH_EQUAL)) {
            binop.type = TOKEN_TYPE_FORWARD_SLASH;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_PERCENT_EQUAL) ) {
            binop.type = TOKEN_TYPE_PERCENT;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_AMPERSAND_EQUAL) ) {
            binop.type = TOKEN_TYPE_AMPERSAND;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_VERTICAL_BAR_EQUAL) ) {
            binop.type = TOKEN_TYPE_VERTICAL_BAR;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_CARROT_EQUAL) ) {
            binop.type = TOKEN_TYPE_CARROT;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_LEFT_SHIFT_EQUAL) ) {
            binop.type = TOKEN_TYPE_LEFT_SHIFT;
        } else if (peek_token(*tokens, token_index, TOKEN_TYPE_RIGHT_SHIFT_EQUAL) ) {
            binop.type = TOKEN_TYPE_RIGHT_SHIFT;
        } else {
            ASSERT(0);
        }

        (*tokens)[token_index].type = TOKEN_TYPE_EQUAL_SIGN;
        array_insert(tokens,(token_index + 2), binop);
    }



    if (peek_token(*tokens, token_index, TOKEN_TYPE_PERCENT) )
        binop_type = PARSE_TYPE_EXP_BINOP_MODULO;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_NEGATE)) 
        binop_type = PARSE_TYPE_EXP_BINOP_SUB;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_ASTERIX))
        binop_type = PARSE_TYPE_EXP_BINOP_MULTIPLY;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_PLUS))
        binop_type = PARSE_TYPE_EXP_BINOP_ADD;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_FORWARD_SLASH))
        binop_type = PARSE_TYPE_EXP_BINOP_DIVIDE;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_VERTICAL_BAR) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_OR;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_AMPERSAND) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_AND;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_CARROT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_XOR;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_LEFT_SHIFT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_RIGHT_SHIFT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_LESS_THAN) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_LESS_THAN_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_GREATER_THAN) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_GREATER_THAN_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_NOT_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_LOGICAL_AND) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_AND;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_LOGICAL_OR) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_OR;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_EQUAL_SIGN) )
        binop_type = PARSE_TYPE_EXP_BINOP_EQUAL;
    else if (peek_token(*tokens, token_index, TOKEN_TYPE_QUESTION_MARK) )
        binop_type = PARSE_TYPE_EXP_CONDITIONAL;



    return binop_type;
}

int get_precedence(Parse_Type binop)
{
    if (binop == PARSE_TYPE_EXP_BINOP_SUB)
        return PRECEDENCE_SUBTRACTION;
    if (binop == PARSE_TYPE_EXP_BINOP_ADD)
        return PRECEDENCE_ADDITION;
    if (binop == PARSE_TYPE_EXP_BINOP_MULTIPLY)
        return PRECEDENCE_MULTIPLY;
    if (binop ==PARSE_TYPE_EXP_BINOP_DIVIDE)
        return PRECEDENCE_DIVIDE;
    if (binop == PARSE_TYPE_EXP_BINOP_MODULO)
        return PRECEDENCE_MODULO;
    if (binop == PARSE_TYPE_EXP_BINOP_BITWISE_OR)
        return PRECEDENCE_BITWISE_OR;
    if (binop == PARSE_TYPE_EXP_BINOP_BITWISE_AND)
        return PRECEDENCE_BITWISE_AND;
    if (binop == PARSE_TYPE_EXP_BINOP_BITWISE_XOR)
        return PRECEDENCE_BITWISE_XOR;
    if (binop == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT)
        return PRECEDENCE_BITWISE_SHIFT_LEFT;
    if (binop == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT)
        return PRECEDENCE_BITWISE_SHIFT_RIGHT;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_AND)
        return PRECEDENCE_LOGICAL_AND;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_OR)
        return PRECEDENCE_LOGICAL_OR;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO)
        return PRECEDENCE_EQUAL_TO;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO)
        return PRECEDENCE_NOT_EQUAL_TO;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN)
        return PRECEDENCE_LESS_THAN;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO)
        return PRECEDENCE_LESS_THAN_EQUAL_TO;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN)
        return PRECEDENCE_GREATER_THAN;
    if (binop == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO)
        return PRECEDENCE_GREATER_THAN_EQUAL_TO;
    if (binop == PARSE_TYPE_EXP_BINOP_EQUAL)
        return PRECEDENCE_EQUAL;
    if (binop == PARSE_TYPE_EXP_CONDITIONAL)
        return PRECEDENCE_CONDITIONAL;
    ASSERT(0);

}

void free_scope_data(Parse_Table *table) {
    if (!table)
        return;
    if (table->function_items) {
        Hash_Map_Header *header = get_hm_header(table->function_items);
        for (int cap_i = 0; cap_i < header->capacity; ++cap_i) {
            if (header->occupied[cap_i]) {
                int param_count = get_array_count(table->function_items[cap_i].params);
                for (int param_i = 0; param_i < param_count; ++param_i) {
                    free_parse_nodes(&table->function_items[cap_i].params[param_i]);
                }
                array_free(&table->function_items[cap_i].params);
            }

        }
        cstr_hash_free(&table->function_items);
    }
    if (table->var_items) cstr_hash_free(&table->var_items);

}


void parse_expression(Parse_Data *data, Parse_Node **expression_root, int min_precedence) 
{

    Parse_Node *left = 0;
    parse_factor(data, &left);
    Parse_Type binop_type = peek_binop_type(data->tokens, data->token_index);
    if (binop_type != PARSE_TYPE_ERROR) {
        int binop_precedence  = get_precedence(binop_type);
        while ( binop_precedence >= min_precedence) {
            Parse_Node *right = 0;
            Parse_Node *middle = 0;
            data->token_index +=1;
            if (binop_type == PARSE_TYPE_EXP_BINOP_EQUAL) {
                ASSERT(left->type == PARSE_TYPE_EXP_FACTOR_VAR);
                parse_expression(data, &right,binop_precedence);
            }else if (binop_type == PARSE_TYPE_EXP_CONDITIONAL) {
                parse_expression(data, &middle, binop_precedence );
                if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_COLON))
                    ASSERT(0);
                (data->token_index) +=1;
                parse_expression(data, &right,  binop_precedence );
            } else {
                parse_expression(data, &right, binop_precedence + 1);
            }

            Parse_Node z = ZERO_STRUCT;
            Parse_Node *nop = 0;
            array_append(&nop, z);
            nop->type = binop_type;
            if (nop->type == PARSE_TYPE_EXP_CONDITIONAL) {
                nop->expression.conditional.condition = left;
                nop->expression.conditional.true_expression = middle;
                nop->expression.conditional.false_expression = right;
            } else {
                nop->expression.binop.left = left;
                nop->expression.binop.right = right;
            }
            left = nop;


            binop_type = peek_binop_type(data->tokens, data->token_index);
            if (binop_type == PARSE_TYPE_ERROR)
                break;
            binop_precedence  = get_precedence(binop_type);
        }
    }
    array_append(expression_root, *left);
    array_free(&left);
}


void parse_statement(Parse_Data *data, Function_Table_Item *function_item, Parse_Node **statement_root) 
{
    if ( peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER) && str_equals_cstr((*data->tokens)[data->token_index].identifier, "return")) {

        Parse_Node return_statement = ZERO_STRUCT;
        return_statement.type = PARSE_TYPE_STATEMENT_RETURN;
        (data->token_index)++;

        parse_expression(data, &return_statement.statement.return_statement.expression,0);

        if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (data->token_index)++;
        array_append(statement_root, return_statement);
    }else if ( peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER) && str_equals_cstr((*data->tokens)[data->token_index].identifier, "goto")) {
        Parse_Node goto_statement = {0};
        goto_statement.type = PARSE_TYPE_STATEMENT_GOTO;
        (data->token_index) ++;
        ASSERT((*data->tokens)[data->token_index].type == TOKEN_TYPE_IDENTIFIER);
        goto_statement.statement.goto_statement.label_identifier = str_clone((*data->tokens)[data->token_index].identifier);
        (data->token_index) ++;
        array_append(statement_root, goto_statement);
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {

        Parse_Node null_statement = ZERO_STRUCT;
        null_statement.type = PARSE_TYPE_STATEMENT_NULL;
        (data->token_index)++;
        array_append(statement_root, null_statement);
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_BREAK)) {
        ASSERT(data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label);
        Parse_Node goto_statement = {0};
        goto_statement.type = PARSE_TYPE_STATEMENT_GOTO;
        goto_statement.statement.goto_statement.label_identifier = str_create_from_cstr(data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label->statement.label_statement.item.key);
        array_append(statement_root, goto_statement);
        (data->token_index) ++;
        if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (data->token_index)++;

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CONTINUE)) {
        ASSERT(data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label);
        Parse_Node goto_statement = {0};
        goto_statement.type = PARSE_TYPE_STATEMENT_GOTO;
        goto_statement.statement.goto_statement.label_identifier = str_create_from_cstr(data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label->statement.label_statement.item.key);
        array_append(statement_root, goto_statement);
        (data->token_index) ++;
        if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (data->token_index)++;
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IF)) {
        Parse_Node if_node = {0};
        if_node.type = PARSE_TYPE_STATEMENT_IF;
        (data->token_index) +=1;
        ASSERT((*data->tokens)[data->token_index].type == TOKEN_TYPE_OPEN_PAREN);
        parse_expression(data, &if_node.statement.if_statement.condition ,0);
        parse_statement(data , function_item, &if_node.statement.if_statement.then );
        if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_ELSE) ) {
            (data->token_index) +=1 ;
            parse_statement(data, function_item, &if_node.statement.if_statement.else_clause);
        } 
        array_append(statement_root, if_node);

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SWITCH)) {
        Parse_Node switch_statement = {0};
        switch_statement.type = PARSE_TYPE_STATEMENT_SWITCH;
        (data->token_index) +=1;

        //Parse_Node *label_continue = data->function_table.continue_label;
        Parse_Node *label_break        = data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label;
        Parse_Node *switch_var         = data->scope_tables[(get_array_count(data->scope_tables)) -1].switch_var;
        Parse_Node **case_statements   = data->scope_tables[(get_array_count(data->scope_tables)) -1].case_statements;
        Parse_Node **default_statement = data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement;
        int default_statement_index    = data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement_index;
        //data->function_table.continue_label = 0;

        Parse_Node new_label_break    = {0};
        new_label_break.type = PARSE_TYPE_STATEMENT_LABEL;
        new_label_break.statement.label_statement.item.key = str_convert_to_cstr(create_label(break_name, loop_id));
        ++loop_id;

        data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label = &new_label_break;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].case_statements = &switch_statement.statement.switch_statement.case_statements;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement  = &switch_statement.statement.switch_statement.default_statement;

        array_append(&switch_statement.statement.switch_statement.break_label_jump, new_label_break);
        if (data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label) {
            Parse_Node continue_label = {0};
            continue_label.type = PARSE_TYPE_STATEMENT_LABEL;
            continue_label.statement.label_statement.item = data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label->statement.label_statement.item;
            array_append(&switch_statement.statement.switch_statement.continue_label_jump, continue_label);
        }

        ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_PAREN));
        parse_expression(data, &switch_statement.statement.switch_statement.switch_var,0);
        ASSERT(switch_statement.statement.switch_statement.switch_var->type == PARSE_TYPE_EXP_FACTOR_EXP);
        ASSERT(switch_statement.statement.switch_statement.switch_var->expression.factor.expression.expression->type == PARSE_TYPE_EXP_FACTOR_VAR);
        data->scope_tables[(get_array_count(data->scope_tables)) -1].switch_var = switch_statement.statement.switch_statement.switch_var;

        ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_BRACKET));
        Parse_Node *body = {0};
        parse_statement(data, function_item ,&body );
        int body_statement_count = get_array_count(body->statement.compound.block_items);
        ASSERT(body_statement_count == 0);
        switch_statement.statement.switch_statement.default_statement_index = data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement_index;

        //node.statement.default_statement.switch_statement_index = get_array_count(data->function_table.case_statements);



        data->scope_tables[(get_array_count(data->scope_tables)) -1].switch_var              = switch_var;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label             = label_break;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].case_statements         = case_statements;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement       = default_statement;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement_index = default_statement_index;

        array_append(statement_root, switch_statement);
        array_free(&body);
    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DEFAULT) || peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CASE)) {
        //static bool default_token = false;
        //ASSERT(default_token == false);

        ASSERT(data->scope_tables[(get_array_count(data->scope_tables)) -1].switch_var);
        ASSERT(data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label);

        bool default_token = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DEFAULT);
        bool case_token = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CASE);
        Parse_Node node = {0};
        if (case_token) {
            node.type = PARSE_TYPE_STATEMENT_CASE;
            //node.statement.case_statement.switch_var = data->scope_tables[(get_array_count(data->scope_tables)) -1].switch_var;
        } else if (default_token){
            ASSERT(!(*data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement));
            node.type = PARSE_TYPE_STATEMENT_DEFAULT;
            data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement_index = get_array_count((*data->scope_tables[(get_array_count(data->scope_tables)) -1].case_statements));
        }
        (data->token_index) +=1;

        if (case_token) {
            parse_expression(data, &node.statement.case_statement.number,0);
            ASSERT(node.statement.case_statement.number->type == PARSE_TYPE_EXP_FACTOR_INT);
        }
        ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_COLON));
        (data->token_index) +=1;

        Parse_Node body_label = {0};
        body_label.type = PARSE_TYPE_STATEMENT_LABEL;
        body_label.statement.label_statement.item.key = str_convert_to_cstr(create_label(body_name, loop_id));
        ++loop_id;

        if (case_token) {
            array_append(&node.statement.case_statement.body_label_jump, body_label);
        } else {
            array_append(&node.statement.default_statement.body_label_jump, body_label);
        }


        bool new_case_token    = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CASE);
        bool new_default_token = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DEFAULT);
        bool new_close_bracket = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_BRACKET);
        bool should_continue   =  !new_case_token && !new_default_token && !new_close_bracket;
        if (case_token) {
            while (should_continue) {
                parse_statement(data, function_item, &node.statement.case_statement.body_statement);
                new_case_token    = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CASE);
                new_default_token = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DEFAULT);
                new_close_bracket = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_BRACKET);
                should_continue   =  !new_case_token && !new_default_token && !new_close_bracket;

            }
            array_append(data->scope_tables[(get_array_count(data->scope_tables)) -1].case_statements, node);
        }else {
            while (should_continue){
                parse_statement(data,function_item,  &node.statement.default_statement.body_statement);
                new_case_token    = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CASE);
                new_default_token = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DEFAULT);
                new_close_bracket = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_BRACKET);
                should_continue   =  !new_case_token && !new_default_token && !new_close_bracket;
            }
            array_append(data->scope_tables[(get_array_count(data->scope_tables)) -1].default_statement, node);
        }




    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_WHILE) || 
            peek_token(*data->tokens, data->token_index, TOKEN_TYPE_FOR)   ||
            peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DO)            
            ) {

        bool found_token_type_while =  peek_token(*data->tokens, data->token_index, TOKEN_TYPE_WHILE) ;
        bool found_token_type_for = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_FOR)  ;
        bool found_token_type_do_while = peek_token(*data->tokens, data->token_index, TOKEN_TYPE_DO);
        Parse_Node loop_node = {0};
        (data->token_index) +=1;
        if (!found_token_type_do_while) 
            ASSERT((*data->tokens)[data->token_index].type == TOKEN_TYPE_OPEN_PAREN);
        Parse_Node *label_continue = data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label;
        Parse_Node *label_break    = data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label;
        Parse_Node new_label_continue = {0};
        new_label_continue.type = PARSE_TYPE_STATEMENT_LABEL;
        new_label_continue.statement.label_statement.item.key = str_convert_to_cstr(create_label(loop_start_name, loop_id));
        Parse_Node new_label_break    = {0};
        new_label_break.type = PARSE_TYPE_STATEMENT_LABEL;
        new_label_break.statement.label_statement.item.key = str_convert_to_cstr(create_label(loop_end_name, loop_id));
        ++loop_id;

        data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label = &new_label_continue;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label = &new_label_break;
        if (found_token_type_while) {
            loop_node.type = PARSE_TYPE_STATEMENT_WHILE;
            array_append(&loop_node.statement.while_statement.continue_label_jump , new_label_continue);
            array_append(&loop_node.statement.while_statement.break_label_jump , new_label_break);
            parse_expression(data, &loop_node.statement.while_statement.condition_expression,0);
            parse_statement(data,function_item,  &loop_node.statement.while_statement.body_statement);
        }else if (found_token_type_do_while) {
            loop_node.type = PARSE_TYPE_STATEMENT_DO_WHILE;
            array_append(&loop_node.statement.do_while_statement.continue_label_jump , new_label_continue);
            array_append(&loop_node.statement.do_while_statement.break_label_jump , new_label_break);
            parse_statement(data,function_item, &loop_node.statement.do_while_statement.body_statement);

            if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_WHILE)) 
                ASSERT(0);
            data->token_index +=1;
            parse_expression(data, &loop_node.statement.do_while_statement.condition_expression,0);
            if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) 
                ASSERT(0);
            data->token_index +=1;
        }else if (found_token_type_for) {
            loop_node.type = PARSE_TYPE_STATEMENT_FOR;
            array_append(&loop_node.statement.for_statement.continue_label_jump , new_label_continue);
            array_append(&loop_node.statement.for_statement.break_label_jump , new_label_break);

            ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_PAREN));
            (data->token_index) +=1;

            //int old_scope_id = data->scope_id;
            //int old_next_scope_id = data->next_scope_id ;
            //increase_scope_id(&data->scope_id, &data->next_scope_id);
            //TODO: add new scope table

            Parse_Table new_scope_table = data->scope_tables[(get_array_count(data->scope_tables)) -1];
            new_scope_table.var_items = 0;
            new_scope_table.function_items = 0;
            array_append(&data->scope_tables, new_scope_table);
            //parse_block_items(data, &compound_statement.statement.compound.block_items);
            //if (data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items)
            //  cstr_hash_free(&data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items);
            //array_add_size_count(&data->scope_tables, -1);

            //int var_item_count  = get_array_count(data->function_table.var_items);
            //for (int i  = var_item_count - 1; i >= 0; --i) {
            //    int var_scope = data->function_table.var_items[i].scope_id;
            //    if (var_scope <= data->scope_id)
            //        break;
            //    //data->function_table.var_items[i].accessible = false;
            //    b32 removed;
            //    cstr_hash_remove(data->function_table.var_items, data->function_table.var_items[i], removed);
            //    ASSERT(removed);
            //} 


            if (peek_declaration_var(data->global_table.valid_type_items, *data->tokens, data->token_index)) {
                parse_declaration_var(data, function_item, &loop_node.statement.for_statement.init);
            } else {
                if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
                    (data->token_index) +=1;
                }else {
                    parse_expression(data, &loop_node.statement.for_statement.init,0);
                    if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
                        (data->token_index) +=1;
                    }else ASSERT(0);
                }
            }

            if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
                (data->token_index) +=1;
            }else {
                parse_expression(data, &loop_node.statement.for_statement.condition_expression,0);
                if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
                    (data->token_index) +=1;
                }else ASSERT(0);
            }

            if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
                (data->token_index) +=1;
            }else {
                parse_expression(data, &loop_node.statement.for_statement.post_expression,0);
                if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_PAREN)) {
                    (data->token_index) +=1;
                }else ASSERT(0);
            }

            //if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_BRACKET)) {
            //    //data->next_scope_id = old_next_scope_id;
            //    parse_statement(data, &loop_node.statement.for_statement.body_statement);
            //} else {
            //    parse_statement(data, &loop_node.statement.for_statement.body_statement);
            //}
            parse_statement(data,function_item, &loop_node.statement.for_statement.body_statement);

//            if (data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items)
//                cstr_hash_free(&data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items);
            int scope_index = get_array_count(data->scope_tables) -1;
            free_scope_data(&data->scope_tables[scope_index]);

            array_add_size_count(&data->scope_tables, -1);




        } else {
            ASSERT(0);
        }

        array_append(statement_root, loop_node);
        data->scope_tables[(get_array_count(data->scope_tables)) -1].continue_label = label_continue;
        data->scope_tables[(get_array_count(data->scope_tables)) -1].break_label = label_break;

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_BRACKET)) {
        //ASSERT(0);
        Parse_Node compound_statement = {0};
        compound_statement.type = PARSE_TYPE_STATEMENT_COMPOUND;
        //int new_scope_id  = data->next_scope_id;
        //data->next_scope_id += 1;

        Parse_Table new_scope_table = data->scope_tables[(get_array_count(data->scope_tables)) -1];
        new_scope_table.var_items = 0;
        new_scope_table.function_items = 0;
        array_append(&data->scope_tables, new_scope_table);
        parse_block_items(data, function_item, &compound_statement.statement.compound.block_items);
//        if (data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items)
//            cstr_hash_free(&data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items);
        int scope_index = get_array_count(data->scope_tables) -1;
        free_scope_data(&data->scope_tables[scope_index]);

        array_add_size_count(&data->scope_tables, -1);

        //int var_item_count  = get_array_count(data->function_table.var_items);
        //for (int i  = var_item_count - 1; i >= 0; --i) {
        //    int var_scope = data->function_table.var_items[i].scope_id;
        //    if (var_scope <= data->scope_id)
        //        break;
        //    //data->function_table.var_items[i].accessible = false;
        //    b32 removed;
        //    cstr_hash_remove(data->function_table.var_items, data->function_table.var_items[i], removed);
        //    ASSERT(removed);
        //} 

        array_append(statement_root,compound_statement);

    }else if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_LABEL)) {
        Parse_Node label_statement = {0};
        label_statement.type = PARSE_TYPE_STATEMENT_LABEL;
        label_statement.statement.label_statement.item.key = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
        int hash_index;
        cstr_hash_find(data->scope_tables[(get_array_count(data->scope_tables)) -1].label_items,  label_statement.statement.label_statement.item.key, hash_index);
        ASSERT(hash_index == -1);
        cstr_hash_add(&data->scope_tables[(get_array_count(data->scope_tables)) -1].label_items, label_statement.statement.label_statement.item);
        //insert_label_table_item(&data->function_table.label_items, label_statement.statement.label_statement.item);
        array_append(statement_root, label_statement);

        (data->token_index) ++;
    } else {

        Parse_Node expression_statement = ZERO_STRUCT;
        expression_statement.type = PARSE_TYPE_STATEMENT_EXPRESSION;
        parse_expression(data, &expression_statement.statement.expression_statement.expression,0);

        if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (data->token_index)++;
        array_append(statement_root, expression_statement);
    }

    return ;
}

Type  get_type(Type *types,  String str) 
{
    int type_count = get_array_count(types);
    for (int i = 0 ; i < type_count; ++i) {
        Type type = types[i];
        if (str_equals_cstr(str, type.key))
            return type;
    }
    ASSERT(0);
}

void parse_declaration_var(Parse_Data *data, Function_Table_Item *function_item, Parse_Node **root) 
{
    if (!peek_token(*data->tokens, data->token_index,TOKEN_TYPE_IDENTIFIER)) {
        ASSERT(0);
    }

    Parse_Node l_value = {0};
    l_value.type = PARSE_TYPE_EXP_FACTOR_VAR;
    l_value.expression.factor.var.item.linkage_type = LINKAGE_TYPE_NONE;
    if (str_equals_cstr((*data->tokens)[data->token_index].identifier, "extern")) {
        l_value.expression.factor.var.item.linkage_type = LINKAGE_TYPE_EXTERNAL;
        data->token_index += 1;
    }

    char *type_cstr = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
    int type_hm_index;
    cstr_hash_find(data->global_table.valid_type_items, type_cstr, type_hm_index);
    if (type_hm_index == -1) ASSERT(0);
    l_value.expression.factor.var.item.param_type = data->global_table.valid_type_items[type_hm_index];
    function_item->stack_count += l_value.expression.factor.var.item.param_type.size;
    l_value.expression.factor.var.item.stack.index = -1 * function_item->stack_count;
    data->token_index += 1;

    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER)) {
        ASSERT(0);
    }
    Parse_Node declaration = {0};
    declaration.type = PARSE_TYPE_STATEMENT_DECLARATION_VAR;
    l_value.expression.factor.var.item.key = str_convert_to_cstr( (*data->tokens)[data->token_index].identifier);
    int invalid_var_name_count = ARRAY_COUNT(invalid_var_names);
    for (int i = 0; i < invalid_var_name_count; ++i) {
        if (cstr_equals_cstr( invalid_var_names[i], l_value.expression.factor.var.item.key))
            ASSERT(0);
    }

    int hash_index;
    cstr_hash_find(data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items, l_value.expression.factor.var.item.key, hash_index);
    if (hash_index != -1) {
        if (data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items[hash_index].linkage_type != LINKAGE_TYPE_EXTERNAL ||
                l_value.expression.factor.var.item.linkage_type != LINKAGE_TYPE_EXTERNAL)  {
            ASSERT(0);
        }
        if (peek_token(*data->tokens, data->token_index + 1, TOKEN_TYPE_EQUAL_SIGN)) {
            ASSERT(0);
        }

    } 

    int scope_index = (get_array_count(data->scope_tables)) -1;
    cstr_hash_find(data->scope_tables[scope_index].function_items, l_value.expression.factor.var.item.key, hash_index);
    if (hash_index != -1) {
            ASSERT(0);
    }
        
    cstr_hash_add(&data->scope_tables[(get_array_count(data->scope_tables)) -1].var_items, l_value.expression.factor.var.item);
    

    data->token_index += 1;
    if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_EQUAL_SIGN)) {
        data->token_index += 1;
        parse_expression(data, &declaration.declaration.var.r_value,0) ;
    } 

    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON))  {
        ASSERT(0);
    }
    data->token_index +=1;
    array_append(&declaration.declaration.var.l_value, l_value);
    array_append(root, declaration);

}

bool peek_declaration_var(Type *valid_type_items, Token *tokens, int token_index) {
    bool declaration = false;
    bool type = peek_token(tokens , token_index, TOKEN_TYPE_IDENTIFIER );
    if (type) {
        int token_index_offset = 0;
        for (int i = 0; i < ARRAY_COUNT(linkage_names); ++i) {
            if (str_equals_cstr(tokens[token_index].identifier, linkage_names[i])) {
                token_index_offset += 1;
                break;
            }
        }
        char *type_name = str_convert_to_cstr(tokens[token_index + token_index_offset].identifier);
        int hash_index;
        cstr_hash_find(valid_type_items, type_name, hash_index);
        if (hash_index == -1)
            goto exit;
        token_index_offset += 1;
        bool var_name =  peek_token(tokens , token_index + token_index_offset, TOKEN_TYPE_IDENTIFIER );
        if (!var_name)
            goto exit;
        for (int i = 0; i < ARRAY_COUNT(invalid_var_names); ++i) {
            if (str_equals_cstr(tokens[token_index + token_index_offset].identifier, invalid_var_names[i]))
                ASSERT(0);
        }

        token_index_offset += 1;
        declaration = tokens[token_index + token_index_offset].type == TOKEN_TYPE_SEMICOLON || tokens[token_index + token_index_offset].type == TOKEN_TYPE_EQUAL_SIGN;
    }
exit:
    return declaration;
}

bool peek_declaration_function(Type *valid_type_items, Token *tokens, int token_index) {
    bool declaration = false;
    bool type = peek_token(tokens , token_index, TOKEN_TYPE_IDENTIFIER );
    if (type) {
        int token_index_offset = 0;
        for (int i = 0; i < ARRAY_COUNT(linkage_names); ++i) {
            if (str_equals_cstr(tokens[token_index].identifier, linkage_names[i])) {
                token_index_offset += 1;
                break;
            }
        }
        char *type_name = str_convert_to_cstr(tokens[token_index + token_index_offset].identifier);
        int hash_index;
        cstr_hash_find(valid_type_items, type_name, hash_index);
        if (hash_index == -1)
            goto exit;
        token_index_offset += 1;
        bool var_name =  peek_token(tokens , token_index + token_index_offset, TOKEN_TYPE_IDENTIFIER );
        if (!var_name)
            goto exit;
        for (int i = 0; i < ARRAY_COUNT(invalid_var_names); ++i) {
            if (str_equals_cstr(tokens[token_index + token_index_offset].identifier, invalid_var_names[i]))
                ASSERT(0);
        }

        token_index_offset += 1;
        declaration = tokens[token_index + token_index_offset].type == TOKEN_TYPE_OPEN_PAREN;
    }
exit:
    return declaration;
}

void parse_block_items(Parse_Data *data, Function_Table_Item *function_item, Parse_Node **root) {
    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_BRACKET)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(data->token_index);

    while (! peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_BRACKET)) {
        while(peek_parse_typedef(&data->scope_tables[(get_array_count(data->scope_tables)) -1], *data->tokens, &data->token_index));
        bool var_declaration = peek_declaration_var(data->global_table.valid_type_items, *data->tokens, data->token_index);
        bool function_declaration = peek_declaration_function(data->global_table.valid_type_items, *data->tokens, data->token_index);
        if (var_declaration) {
                int hash_index ;
                char *key = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
                cstr_hash_find(data->global_table.valid_type_items,  key,  hash_index);

                if (hash_index == -1) {
                    cstr_hash_find(data->scope_tables[(get_array_count(data->scope_tables)) -1].typedef_items,  key,  hash_index);
                    if (hash_index == -1) 
                        ASSERT(0);
                    Typedef_Table_Item item = data->scope_tables[(get_array_count(data->scope_tables)) -1].typedef_items[hash_index];
                    (*data->tokens)[data->token_index].identifier =  str_create_from_cstr(item.key);
                }


                parse_declaration_var(data,function_item, root);
            

        }else if (function_declaration) {
                int current_scope_table = get_array_count(data->scope_tables) -1;
                parse_function_declaration(data, &data->scope_tables[current_scope_table]);
        }else{

            parse_statement(data, function_item, root);
        }

    }
    ++(data->token_index);

}

void insert_function_table(Parse_Table *global_table, Function_Table_Item *function) {

    int hash_index;
    cstr_hash_find(global_table->function_items, function->key, hash_index);
    if (hash_index == -1) {
        cstr_hash_add(&global_table->function_items, (*function));
    } else {
        int param_count  = get_array_count(function->params);
        int table_param_count = get_array_count(global_table->function_items[hash_index].params);
        ASSERT(param_count == table_param_count);
        for (int i=0; i < param_count; ++i) {
            if (!cstr_equals_cstr(function->params[i].parameter.type.key , global_table->function_items[hash_index].params[i].parameter.type.key))
                ASSERT(0);
        }

        if (function->block_items) {
            if (function->params != global_table->function_items[hash_index].params) {
                array_free(&global_table->function_items[hash_index].params);
            }

            ASSERT(!global_table->function_items[hash_index].block_items);
            char *key = global_table->function_items[hash_index].key;
            //printf(" KE_ %s\n",global_table->function_items[hash_index].key );
            global_table->function_items[hash_index] = *function;
            //printf(" KE_ %s\n",global_table->function_items[hash_index].key );
            global_table->function_items[hash_index].key = key;

           // free(global_table->function_items[hash_index].key = key;
        }
    }

}



void parse_function_declaration(Parse_Data *data, Parse_Table *current_scope_table) 
{
    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER)) {
        FAIL_MSG("expected function return type\n");
    }

    Function_Table_Item function_item  = ZERO_STRUCT;

    char *return_value_name = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
    int hash_index ;
    cstr_hash_find(data->global_table.valid_type_items, return_value_name, hash_index);
    ASSERT(hash_index != -1);
    function_item.return_type.key = return_value_name;
    ++(data->token_index);

    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER)) {
        FAIL_MSG( "expected function return type\n");
    }
    char *function_name = str_convert_to_cstr( (*data->tokens)[data->token_index].identifier);
    //cstr_hash_find(data->global_table.valid_type_items, function_name, hash_index);
    function_item.key  = function_name;
    ++(data->token_index);

    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_OPEN_PAREN)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(data->token_index);

    if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER)) {
        if (!str_equals_cstr((*data->tokens)[data->token_index].identifier, "void")) {
            while (1) {
                ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER));
                Parse_Node param = {0};
                param.type = PARSE_TYPE_PARAMETER;
                param.parameter.type.key = str_convert_to_cstr((*data->tokens)[data->token_index].identifier);
                int hash_index ;
                cstr_hash_find(data->global_table.valid_type_items, param.parameter.type.key, hash_index);
                ASSERT(hash_index != -1);
                param.parameter.type = data->global_table.valid_type_items[hash_index];
                ++(data->token_index);

                ASSERT(peek_token(*data->tokens, data->token_index, TOKEN_TYPE_IDENTIFIER));
                param.parameter.name = str_clone((*data->tokens)[data->token_index].identifier);
                int param_count = get_array_count(function_item.params);
                for (int i =0 ; i  < param_count; ++i) {
                    ASSERT(!str_equals_str(param.parameter.name, function_item.params[i].parameter.name));

                }
                array_append(&function_item.params, param);
                ++(data->token_index);
                if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_PAREN)) {
                    break;
                }
                if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_COMMA))
                    ASSERT(0);
                ++(data->token_index);

            }
        }else {
            ++(data->token_index);
        }
    }


    if (!peek_token(*data->tokens, data->token_index, TOKEN_TYPE_CLOSE_PAREN)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(data->token_index);
    cstr_hash_find(current_scope_table->var_items, function_name, hash_index);
    ASSERT(hash_index == -1);

    insert_function_table(current_scope_table, &function_item);
    if (peek_token(*data->tokens, data->token_index, TOKEN_TYPE_SEMICOLON)) {
        data->token_index += 1;
        goto exit;
    }
    if (current_scope_table != &data->global_table) {
        ASSERT(0);
    }



    Parse_Table scope_table = {0};
    array_append(&data->scope_tables, scope_table);

    int param_count = get_array_count(function_item.params);
    for (int i = 0 ; i < param_count; ++i) {
        Var_Table_Item item = {0};
        Parse_Node *param = &function_item.params[i];
        item.param_type = param->parameter.type;
        item.key = str_convert_to_cstr(param->parameter.name);
        item.linkage_type = LINKAGE_TYPE_NONE;
        item.type = VAR_TABLE_ITEM_TYPE_STACK;

        function_item.stack_count += 8;
        item.stack.index =  -1 * function_item.stack_count;
        int scope_index = get_array_count(data->scope_tables) -1;
        cstr_hash_add((&(data->scope_tables[scope_index].var_items)), item);

        //}
    }



    parse_block_items( data, &function_item, &function_item.block_items);
    int scope_index = (get_array_count(data->scope_tables)) -1;
    free_scope_data(&data->scope_tables[scope_index]);
    //array_free(&data->scope_tables);
    int scope_count = get_array_count(data->scope_tables);
    for (int scope_i = 0; scope_i < scope_count; ++scope_i) {
        Parse_Table *table = &data->scope_tables[scope_i];

            free_scope_data(table);
    }

   //     array _free(&data->scope_tables);
        array_free(&data->scope_tables);

    insert_function_table(&data->global_table, &function_item);


exit:
    //cstr_hash_add(&data->global_table.function_items, function_item);

    return ;
}

Parse_Node *parse_tokens( Token **tokens) 
{
    Parse_Node program = ZERO_STRUCT;

    program.type = PARSE_TYPE_PROGRAM;
    Type type_int = {0};
    type_int.key = "int";
    type_int.size = 4;

    cstr_hash_add(&program.program.data.global_table.valid_type_items, type_int);

    program.program.data.token_index = 0;
    program.program.data.tokens = tokens;

    int total_tokens = get_array_count(*program.program.data.tokens);
    while (total_tokens != program.program.data.token_index) {
        parse_function_declaration(&program.program.data, &program.program.data.global_table);
        total_tokens = get_array_count(*program.program.data.tokens);
    }

    Hash_Map_Header *header  =get_hm_header(program.program.data.global_table.function_items);

    Parse_Node *rv =0;
    array_append(&rv, program);

    return rv;
}
