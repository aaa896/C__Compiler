#include <stdio.h>

#include "lex.h"
#include "parse.h"
#include "dynamic_array.h"
#include "string.h"

char *invalid_var_names[] = {
    "int" ,
    "void",
    "return",
};

int peek_token(Token *tokens, int token_index, Token_Type type) ;
void parse_expression(Parse_Node **expression_root, Var_Table *global_table, Var_Table *table, Token *tokens, int *token_index, int min_precedence) ;

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
        int function_count = get_array_count(node->program.functions);
        for (int i = 0; i < function_count; ++i) {
            print_parse_nodes_2(&node->program.functions[i], start_padding + padding_increment, padding_increment);
        }
        printf("}\n");
    }else if (node->type == PARSE_TYPE_FUNCTION) {
        print_char_n(' ', start_padding);
        //printf("FN rv:" SV_FMT " name:"SV_FMT" {\n", SV_ARG(node->function.return_value_name), SV_ARG(node->function.name));
        printf("FN rv: ");
        str_print(node->function.return_value_name);
        printf(" name:");
        str_print(node->function.name);
        printf("{\n");
        if (node->function.block_items) {
            int block_item_count = get_array_count(node->function.block_items);
            for (int i = 0; i  < block_item_count; ++i) {
                print_parse_nodes_2(&node->function.block_items[i], start_padding + padding_increment, padding_increment);
            }
        }
        print_char_n(' ', start_padding);
        printf("}\n");
    }else if (node->type == PARSE_TYPE_STATEMENT_RETURN)      {
        print_char_n(' ', start_padding);
        printf("Statement: Return (\n");
        print_parse_nodes_2(node->statement.expression, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_STATEMENT_NULL)      {
        print_char_n(' ', start_padding);
        printf("Statement: Null; \n");
    }else if (node->type == PARSE_TYPE_STATEMENT_EXPRESSION)      {
        print_char_n(' ', start_padding);
        printf("Statement: Expression (\n");
        print_parse_nodes_2(node->statement.expression, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");

    }else if (node->type == PARSE_TYPE_EXP_FACTOR_INT) {
        print_char_n(' ', start_padding);
        printf("Int %d \n", node->expression.factor.int_value);
        printf("\n");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_VAR) {
        print_char_n(' ', start_padding);
        printf("Var :");
        str_print(node->expression.factor.var_name);
        printf(" %d", node->expression.factor.int_value);
        printf("\n");

    }else if (node->type == PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT) {
        print_char_n(' ', start_padding);
        printf("Pre Increment ++(\n");
        print_parse_nodes_2(node->expression.factor.increment_value, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_POST_INCREMENT) {
        print_char_n(' ', start_padding);
        printf("Post Increment ++(\n");
        print_parse_nodes_2(node->expression.factor.increment_value, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE) {
        print_char_n(' ', start_padding);
        printf("Unop Negate -(\n");
        print_parse_nodes_2(node->expression.factor.unop_next, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_EXP) {

        print_parse_nodes_2(node->expression.factor.expression, start_padding, padding_increment);
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT) {
        print_char_n(' ', start_padding);
        printf("Unop Bitwise ~(\n ");
        print_parse_nodes_2(node->expression.factor.unop_next, start_padding + padding_increment, padding_increment);
        print_char_n(' ', start_padding);
        printf(" )\n ");
    }else if (node->type == PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT) {
        print_char_n(' ', start_padding);
        printf("Unop Logical Not !(\n ");
        print_parse_nodes_2(node->expression.factor.unop_next, start_padding + padding_increment, padding_increment);
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

    }else if (node->type == PARSE_TYPE_STATEMENT_DECLARATION) {
        print_char_n(' ', start_padding);
        printf("Declaration  (\n ");
        print_parse_nodes_2(node->declaration.l_value,  start_padding + padding_increment, padding_increment);
        if (node->declaration.r_value) {
            print_parse_nodes_2(node->declaration.r_value,  start_padding + padding_increment, padding_increment);
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


b32 peek_parse_typedef(Var_Table *global_table, Token *tokens, int *token_index) {
    if (peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER) && str_equals_cstr(tokens[*token_index].identifier, "typedef")) {
        *token_index +=1;
        Typedef_Item item = {0};
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER))
            ASSERT(0);
        item.type = str_clone(tokens[*token_index].identifier);
        *token_index +=1;
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER))
            ASSERT(0);
        item.def = str_clone(tokens[*token_index].identifier);
        *token_index +=1;
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON))
            ASSERT(0);
        *token_index +=1;
        array_append(&global_table->typedef_items, item);

        return true;
    }
    return false;
}

int get_var_table_index(Var_Table *var_table, String identifier) {
    if (var_table->var_items) {
        int var_table_count = get_array_count(var_table->var_items);
        for (int i = 0; i < var_table_count; ++i) {
            String table_string = var_table->var_items[i].string;
            if (str_equals_str(table_string, identifier))
                return i;
        }
    }
    return -1;
}

void insert_var_table(Var_Table *var_table, String identifier) {
    Var_Table_Item item = {0};
    item.string = str_clone(identifier);
    int array_count = get_array_count(var_table->var_items);
    item.stack_index = array_count;
    array_append(&var_table->var_items, item);
}


int peek_token(Token *tokens, int token_index, Token_Type type) 
{
    if ((int)get_array_count(tokens) <= token_index)
        return 0;
    return (tokens[token_index].type == type);
}


void parse_factor(Parse_Node **factor_root,  Var_Table *global_table, Var_Table *var_table, Token *tokens, int *token_index)
{
    if (peek_token(tokens, *token_index, TOKEN_TYPE_NUMBER)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_INT;
        factor.expression.factor.int_value = tokens[*token_index].constant;
        factor.expression.factor.var_name = tokens[*token_index].identifier;
        array_append(factor_root, factor);
        (*token_index)++;

    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_NEGATE)) {

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE;
        (*token_index)++;
        parse_factor(&(factor.expression.factor.unop_next),global_table, var_table , tokens, token_index);
        array_append(factor_root, factor);

    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER)) {

        bool post_increment = false;
        {
            ASSERT(*token_index < get_array_count(tokens) -1);
            Token increment = tokens[*token_index + 1];
            post_increment = increment.type == TOKEN_TYPE_INCREMENT;
        }

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_VAR;
        factor.expression.factor.var_name = tokens[*token_index].identifier;
        int var_table_index = get_var_table_index(var_table, factor.expression.factor.var_name);
        ASSERT(var_table_index != -1);
        factor.expression.factor.int_value = var_table_index;

        if (post_increment) {
            Parse_Node post = {0};
            post.type = PARSE_TYPE_EXP_FACTOR_POST_INCREMENT;
            array_append(&post.expression.factor.increment_value, factor);
            array_append(factor_root, post);
            (*token_index)++;
        } else {
            array_append(factor_root, factor);
        }
        (*token_index)++;

    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_INCREMENT)) {

        bool pre_increment = false;
        {
            ASSERT(*token_index < get_array_count(tokens) -1);
            String var_str = tokens[*token_index + 1].identifier;
            int var_table_index = get_var_table_index(var_table, var_str);
            if (var_table_index != -1) {
                pre_increment = true;
            }
        }
        ASSERT(pre_increment);
        *(token_index) +=1;

        Parse_Node pre = {0};
        pre.type = PARSE_TYPE_EXP_FACTOR_PRE_INCREMENT;

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_VAR;
        factor.expression.factor.var_name = tokens[*token_index].identifier;
        int var_table_index = get_var_table_index(var_table, factor.expression.factor.var_name);
        ASSERT(var_table_index != -1);
        factor.expression.factor.int_value = var_table_index;

        array_append(&pre.expression.factor.increment_value, factor );
        array_append(factor_root, pre);
        *(token_index) +=1;
    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_TILDE)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT;
        (*token_index)++;
        parse_factor(&(factor.expression.factor.unop_next),global_table, var_table ,  tokens, token_index);
        array_append(factor_root, factor);
    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_EXCLAMATION)) {
        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT;
        (*token_index)++;
        parse_factor(&(factor.expression.factor.unop_next),global_table, var_table ,  tokens, token_index);
        array_append(factor_root, factor);
    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_OPEN_PAREN)) {

        Parse_Node factor = ZERO_STRUCT;
        factor.type = PARSE_TYPE_EXP_FACTOR_EXP;
        (*token_index)++;
        parse_expression(&(factor.expression.factor.expression), global_table, var_table, tokens, token_index, 0);
        if (!peek_token(tokens, *token_index, TOKEN_TYPE_CLOSE_PAREN)) {
            FAIL_MSG("expected exp number type\n");
        }
        (*token_index)++;
        array_append(factor_root, factor);
    }else {
        FAIL_MSG(" Uknown factor\n");
    }

}



Parse_Type peek_binop_type(Token *tokens, int token_index) 
{
    Parse_Type binop_type = PARSE_TYPE_ERROR;


    if (peek_token(tokens, token_index, TOKEN_TYPE_PLUS_EQUAL)                ||
            peek_token(tokens, token_index, TOKEN_TYPE_ASTERIX_EQUAL)         ||
            peek_token(tokens, token_index, TOKEN_TYPE_FORWARD_SLASH_EQUAL)   ||
            peek_token(tokens, token_index, TOKEN_TYPE_PERCENT_EQUAL)         ||
            peek_token(tokens, token_index, TOKEN_TYPE_AMPERSAND_EQUAL)       ||
            peek_token(tokens, token_index,   TOKEN_TYPE_VERTICAL_BAR_EQUAL ) ||
            peek_token(tokens, token_index,TOKEN_TYPE_CARROT_EQUAL)           ||
            peek_token(tokens, token_index,TOKEN_TYPE_LEFT_SHIFT_EQUAL)       ||
            peek_token(tokens, token_index,TOKEN_TYPE_RIGHT_SHIFT_EQUAL) )
     {
         Token binop = {0};
         ASSERT(token_index >= 1);
         Token identifier = tokens[token_index -1];
         ASSERT(identifier.type == TOKEN_TYPE_IDENTIFIER);
         array_insert(&tokens,token_index + 1, identifier);
         if (peek_token(tokens, token_index, TOKEN_TYPE_PLUS_EQUAL)) {
             binop.type = TOKEN_TYPE_PLUS;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_ASTERIX_EQUAL)) {
             binop.type = TOKEN_TYPE_ASTERIX;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_FORWARD_SLASH_EQUAL)) {
             binop.type = TOKEN_TYPE_FORWARD_SLASH;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_PERCENT_EQUAL) ) {
             binop.type = TOKEN_TYPE_PERCENT;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_AMPERSAND_EQUAL) ) {
             binop.type = TOKEN_TYPE_AMPERSAND;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_VERTICAL_BAR_EQUAL) ) {
             binop.type = TOKEN_TYPE_VERTICAL_BAR;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_CARROT_EQUAL) ) {
             binop.type = TOKEN_TYPE_CARROT;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_LEFT_SHIFT_EQUAL) ) {
             binop.type = TOKEN_TYPE_LEFT_SHIFT;
         } else if (peek_token(tokens, token_index, TOKEN_TYPE_RIGHT_SHIFT_EQUAL) ) {
             binop.type = TOKEN_TYPE_RIGHT_SHIFT;
         } else {
             ASSERT(0);
         }

         tokens[token_index].type = TOKEN_TYPE_EQUAL_SIGN;
         array_insert(&tokens,token_index + 2, binop);
    }



    if (peek_token(tokens, token_index, TOKEN_TYPE_PERCENT) )
        binop_type = PARSE_TYPE_EXP_BINOP_MODULO;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_NEGATE)) 
        binop_type = PARSE_TYPE_EXP_BINOP_SUB;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_ASTERIX))
        binop_type = PARSE_TYPE_EXP_BINOP_MULTIPLY;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_PLUS))
        binop_type = PARSE_TYPE_EXP_BINOP_ADD;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_FORWARD_SLASH))
        binop_type = PARSE_TYPE_EXP_BINOP_DIVIDE;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_VERTICAL_BAR) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_OR;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_AMPERSAND) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_AND;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_CARROT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_XOR;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_LEFT_SHIFT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_RIGHT_SHIFT) )
        binop_type = PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_LESS_THAN) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_LESS_THAN_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_GREATER_THAN) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_GREATER_THAN_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_NOT_EQUAL_TO) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_LOGICAL_AND) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_AND;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_LOGICAL_OR) )
        binop_type = PARSE_TYPE_EXP_BINOP_LOGICAL_OR;
    else if (peek_token(tokens, token_index, TOKEN_TYPE_EQUAL_SIGN) )
        binop_type = PARSE_TYPE_EXP_BINOP_EQUAL;





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
    ASSERT(0);

}


void parse_expression(Parse_Node **expression_root, Var_Table *global_table, Var_Table *var_table, Token *tokens, int *token_index, int min_precedence) 
{

    Parse_Node *left = 0;
    parse_factor(&left, global_table, var_table, tokens, token_index);
    Parse_Type binop_type = peek_binop_type(tokens, *token_index);
    if (binop_type != PARSE_TYPE_ERROR) {
        int binop_precedence  = get_precedence(binop_type);
        //       if (binop_type != PARSE_TYPE_ERROR) {
        while ( binop_precedence >= min_precedence) {
            Parse_Node *right = 0;
            *token_index +=1;
            if (binop_type == PARSE_TYPE_EXP_BINOP_EQUAL) {
                ASSERT(left->type == PARSE_TYPE_EXP_FACTOR_VAR);
                parse_expression(&right,global_table, var_table, tokens, token_index, binop_precedence );
            } else {
                parse_expression(&right, global_table, var_table, tokens, token_index, binop_precedence + 1);
            }

            Parse_Node z = ZERO_STRUCT;
            Parse_Node *binop = 0;
            array_append(&binop, z);
            binop->type = binop_type;
            binop->expression.binop.left = left;
            binop->expression.binop.right = right;
            left = binop;


            binop_type = peek_binop_type(tokens, *token_index);
            if (binop_type == PARSE_TYPE_ERROR)
                break;
            binop_precedence  = get_precedence(binop_type);
        }
        //}

    }
    array_append(expression_root, *left);
}


void parse_statement(Parse_Node **statements, Var_Table *global_table, Var_Table *var_table, Token *tokens, int *token_index) 
{
    if ( peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER) && str_equals_cstr(tokens[*token_index].identifier, "return")) {

        Parse_Node return_statement = ZERO_STRUCT;
        return_statement.type = PARSE_TYPE_STATEMENT_RETURN;
        (*token_index)++;

        parse_expression(&return_statement.statement.expression,global_table, var_table, tokens, token_index,0);

        if (!peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (*token_index)++;
        array_append(statements, return_statement);
    }else if (peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON)) {

        Parse_Node null_statement = ZERO_STRUCT;
        null_statement.type = PARSE_TYPE_STATEMENT_NULL;
        (*token_index)++;
        array_append(statements, null_statement);
    } else {

        Parse_Node expression_statement = ZERO_STRUCT;
        expression_statement.type = PARSE_TYPE_STATEMENT_EXPRESSION;
        parse_expression(&expression_statement.statement.expression,global_table, var_table,  tokens, token_index,0);

        if (!peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON)) {
            FAIL_MSG( "expected statement end ; type\n");
        }
        (*token_index)++;
        array_append(statements, expression_statement);
    }

    return ;
}

void parse_declaration( Parse_Node **block_item_root, Var_Table *global_table, Var_Table *var_table, Token *tokens, int *token_index) 
{
    if (!peek_token(tokens, *token_index,TOKEN_TYPE_IDENTIFIER)) {
        ASSERT(0);
    }
    if (!str_equals_cstr(tokens[*token_index].identifier, "int")) {
        ASSERT(0);
    }
    *token_index += 1;

    if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER)) {
        ASSERT(0);
    }
    Parse_Node declaration = {0};
    declaration.type = PARSE_TYPE_STATEMENT_DECLARATION;
    Parse_Node l_value = {0};
    l_value.type = PARSE_TYPE_EXP_FACTOR_VAR;
    l_value.expression.factor.var_name = str_clone( tokens[*token_index].identifier);
    for (int i = 0; i < ARRAY_COUNT(invalid_var_names); ++i) {
        if (str_equals_cstr(l_value.expression.factor.var_name, invalid_var_names[i]))
            ASSERT(0);
    }
    l_value.expression.factor.int_value = get_array_count(var_table->var_items);
    int var_table_index = get_var_table_index(var_table, l_value.expression.factor.var_name);
    if (var_table_index != -1)
        ASSERT(0);
    insert_var_table(var_table, l_value.expression.factor.var_name);

    *token_index += 1;
    if (peek_token(tokens, *token_index, TOKEN_TYPE_EQUAL_SIGN)) {
        *token_index += 1;
        parse_expression(&declaration.declaration.r_value, global_table, var_table,  tokens, token_index, 0) ;
    } 

    if (!peek_token(tokens, *token_index, TOKEN_TYPE_SEMICOLON))  {
        ASSERT(0);
    }
    *token_index +=1;
    array_append(&declaration.declaration.l_value, l_value);
    array_append(block_item_root, declaration);

}

void parse_block_items( Parse_Node **block_item_root, Var_Table *global_table, Var_Table *var_table, Token *tokens, int *token_index) {
    if (!peek_token(tokens, *token_index, TOKEN_TYPE_OPEN_BRACKET)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(*token_index);

    while (! peek_token(tokens, *token_index, TOKEN_TYPE_CLOSE_BRACKET)) {
        while(peek_parse_typedef(var_table, tokens, token_index));
        bool identifier =  peek_token(tokens , *token_index, TOKEN_TYPE_IDENTIFIER );
        bool second_identifier =  peek_token(tokens , *token_index + 1, TOKEN_TYPE_IDENTIFIER );
        bool declaration = identifier && (!str_equals_cstr(tokens[*token_index].identifier, "return")) && second_identifier;
        if (declaration) {
            if (!str_equals_cstr(tokens[*token_index].identifier, "int")) {
                //TODO fix typedef working as asign
                String type_def = {0};
                int local_table_count = get_array_count(var_table->typedef_items);
                bool match_type = false;
                for (int i = 0 ; i < local_table_count; ++i) {
                    Typedef_Item item = var_table->typedef_items[i];
                    if (str_equals_str(item.def, tokens[*token_index].identifier)) {
                        if (str_equals_cstr(item.type, "int")) {
                            match_type = true;
                            tokens[*token_index].identifier =  str_create_from_cstr("int");
                            break;
                        } else {
                            ASSERT(0);
                        }
                    }
                }
                if (!match_type) {
                    String type_def = {0};
                    int global_table_count = get_array_count(global_table->typedef_items);
                    for (int i = 0 ; i < global_table_count; ++i) {
                        Typedef_Item item = global_table->typedef_items[i];
                        if (str_equals_str(item.def, tokens[*token_index].identifier)) {
                            if (str_equals_cstr(item.type, "int")) {
                                match_type  = true;
                                tokens[*token_index].identifier =  str_create_from_cstr("int");
                                break;
                            } else {
                                ASSERT(0);
                            }
                        }
                    }
                }
                if (!match_type) 
                    ASSERT(0);
            }
            parse_declaration(block_item_root, global_table, var_table, tokens, token_index);

        }else {
            parse_statement(block_item_root,global_table, var_table, tokens, token_index);
        }
        int total_tokens = get_array_count(tokens);
        if (total_tokens <= *token_index) {
            ASSERT(0);
        }

    }
    ++(*token_index);

}

void parse_function(Var_Table *global_table, Parse_Node **functions, Token *tokens, int *token_index) 
{
    if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER)) {
        FAIL_MSG("expected function return type\n");
    }
    Parse_Node function  = ZERO_STRUCT;
    function.type = PARSE_TYPE_FUNCTION;
    function.function.return_value_name = str_clone( tokens[*token_index].identifier);
    ++(*token_index);

    if (!peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER)) {
        FAIL_MSG( "expected function return type\n");
    }
    function.function.name = str_clone( tokens[*token_index].identifier);
    ++(*token_index);

    if (!peek_token(tokens, *token_index, TOKEN_TYPE_OPEN_PAREN)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(*token_index);

    if (peek_token(tokens, *token_index, TOKEN_TYPE_IDENTIFIER)) {
        if (!str_equals_cstr(tokens[*token_index].identifier, "void")) {
            FAIL_MSG("Function arg non void\n");
        }
        ++(*token_index);
    }


    if (!peek_token(tokens, *token_index, TOKEN_TYPE_CLOSE_PAREN)) {
        FAIL_MSG("expected function ( \n");
    }
    ++(*token_index);


    parse_block_items( &function.function.block_items, global_table,&function.function.var_table, tokens, token_index);

    //if (!peek_token(tokens, *token_index, TOKEN_TYPE_CLOSE_BRACKET)) {
    //    FAIL_MSG("expected function } \n");
    //}
    array_append(functions, function);

    return ;
}

Parse_Node * parse_program( Token *tokens) 
{
    Parse_Node program = ZERO_STRUCT;
    program.type = PARSE_TYPE_PROGRAM;
    int token_index = 0;
    while(peek_parse_typedef(&program.program.global_table, tokens, &token_index));
    parse_function( &program.program.global_table, &program.program.functions, tokens, &token_index);
    int total_tokens = get_array_count(tokens);
    if (total_tokens != token_index) {
        FAIL_MSG("Not all tokens parsed\n");
    }

    Parse_Node *rv =0;
    array_append(&rv, program);
    return rv;
}



Parse_Node *parse_tokens( Token *tokens) 
{
    Parse_Node *parse_nodes =  parse_program( tokens);
    return parse_nodes;
}
