#include <stdio.h>
#include "parse.h"
#include "tac.h"
#include "string.h"
#include "dynamic_array.h"



static char * var_name = "tmp.";
static char * function_var_name = "fn.var.";
int var_name_index  = 0;

static char * label_name       = "label.";
static int    label_name_index = 0;

Tac_Node_Operand parse_var_to_ir( Parse_Node *parse_var) ;
void print_n_char(int x, char c)
{
    for (int i = 0; i < x; ++i) 
        putchar(c);
}

void print_tac_operand(Tac_Node_Operand *operand) 
{
    if (operand->type == TAC_NODE_OPERAND_INT) {
        printf("Int:%d ",operand->int_value);
    } else if (operand->type == TAC_NODE_OPERAND_VAR) {
        printf("Var:");
        str_print(operand->identifier);

    } else if (operand->type == TAC_NODE_INSTRUCTION_LABEL) {
        printf("Label:");
        str_print(operand->identifier);
    }else {
        FAIL_MSG("unknown print operand");
    }
}

void print_tac_nodes_pad(Tac_Node *node, int start_pad, int pad_increment)
{
    if (node->type == TAC_NODE_PROGRAM) {
        print_n_char(start_pad, ' ');
        printf("Tac Program {\n");
        int function_count = get_array_count(node->program.functions);
        for (int i = 0; i < function_count; ++i) {
            print_tac_nodes_pad(&node->program.functions[i], start_pad + pad_increment, pad_increment);
        }
        printf("}\n");
    } else if (node->type == TAC_NODE_FUNCTION){
        print_n_char(start_pad, ' ');
        //printf(SV_FMT" "SV_FMT"{\n", SV_ARG(node->function.return_value_name),SV_ARG(node->function.name));
        str_print(node->function.return_value_name);
        printf(" ");
        str_print(node->function.name);
        printf("{\n");
        int instruction_count = get_array_count(node->function.instructions);
        for (int i = 0; i < instruction_count; ++i) {
            print_tac_nodes_pad(&node->function.instructions[i], start_pad + pad_increment, pad_increment);
        }
        print_n_char(start_pad, ' ');
        printf("}\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_RETURN) {
        print_n_char(start_pad, ' ');
        printf("Return(");
        print_tac_operand(&node->instruction.return_operand);
        printf(")\n");

    } else if (node->type == TAC_NODE_INSTRUCTION_LABEL) {
        print_n_char(start_pad, ' ');
        printf("Label(");
        print_tac_operand(&node->instruction.label.operand);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_JINZ) {
        print_n_char(start_pad, ' ');
        printf("Jinz(");
        print_tac_operand(&node->instruction.jmp.condition);
        printf(",");
        print_tac_operand(&node->instruction.jmp.label);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_JIZ) {
        print_n_char(start_pad, ' ');
        printf("Jiz(");
        print_tac_operand(&node->instruction.jmp.condition);
        printf(",");
        print_tac_operand(&node->instruction.jmp.label);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_JMP) {
        print_n_char(start_pad, ' ');
        printf("Jmp(");
        print_tac_operand(&node->instruction.jmp.label);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_UNARY_NEGATE) {
        print_n_char(start_pad, ' ');
        printf("Unary negate - (");
        print_tac_operand(&node->instruction.unary.src);
        printf(",");
        print_tac_operand(&node->instruction.unary.dest);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_UNARY_BITWISE_NOT) {
        print_n_char(start_pad, ' ');
        printf("Unary bitwise ~ (");
        print_tac_operand(&node->instruction.unary.src);
        printf(",");
        print_tac_operand(&node->instruction.unary.dest);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_UNARY_LOGICAL_NOT) {
        print_n_char(start_pad, ' ');
        printf("Logical Not ! (");
        print_tac_operand(&node->instruction.unary.src);
        printf(",");
        print_tac_operand(&node->instruction.unary.dest);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_COPY) {
        print_n_char(start_pad, ' ');
        printf("Copy  (");
        print_tac_operand(&node->instruction.copy.src);
        printf(",");
        print_tac_operand(&node->instruction.copy.dest);
        printf(")\n");
    } else if (node->type == TAC_NODE_INSTRUCTION_BINARY_ADD
            ||node->type == TAC_NODE_INSTRUCTION_BINARY_SUB
            || node->type == TAC_NODE_INSTRUCTION_BINARY_MULTIPLY
            ||node->type == TAC_NODE_INSTRUCTION_BINARY_DIVIDE
            ||node->type == TAC_NODE_INSTRUCTION_BINARY_MODULO
            || node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR
            || node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND
            || node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR
            || node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT
            || node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT
    || node->type == TAC_NODE_INSTRUCTION_BINARY_LESS_THAN
    || node->type == TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO
    || node->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN
    || node->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO
    || node->type == TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO
    || node->type == TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO) {

        print_n_char(start_pad, ' ');

     if (node->type == TAC_NODE_INSTRUCTION_BINARY_ADD)
        printf("Binary Add + (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_SUB)
         printf("Binary Sub - (");
     else if( node->type == TAC_NODE_INSTRUCTION_BINARY_MULTIPLY)
         printf("Binary Multiply * (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_DIVIDE)
         printf("Binary Divide / (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_MODULO) 
         printf("Binary Modulo %% (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR)
         printf("Binary Bitwise Or | (");
     else if ( node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND)
         printf("Binary Bitwise And &(");
     else if ( node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR)
         printf("Binary Bitwise Xor ^(");
     else if ( node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT)
         printf("Binary Bitwise Shift Left <<(");
     else if ( node->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT) 
         printf("Binary Bitwise Shift Right >>(");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_LESS_THAN)
         printf("Binary Less Than < (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO)
         printf("Binary Less Than Eq <= (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN)
         printf("Binary Greater Than > (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO)
         printf("Binary Greater Than EQ >= (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO)
         printf("Binary Equal to == (");
     else if (node->type == TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO)
         printf("Binary Not Equal to != (");
     else 
         ASSERT(0);

     print_tac_operand(&node->instruction.binary.src1);
     printf(",");
     print_tac_operand(&node->instruction.binary.src2);
     printf(",");
     print_tac_operand(&node->instruction.binary.dest);
     printf(")\n");

    } else if (node->type == TAC_NODE_OPERAND_INT) {
    } else if (node->type == TAC_NODE_OPERAND_VAR) {
    }else {
        FAIL_MSG( "unknown Tac node print");
    }

}

void print_tac_nodes(Tac_Node *node)
{
    int padding_increment = 4;
    int start_pad = 0;
    print_tac_nodes_pad(node, start_pad, padding_increment);
}

Tac_Node_Operand create_tac_instructions( Tac_Node **tac_instructions, Parse_Node *parse_expression)
{
    Tac_Node tac_node = ZERO_STRUCT;
    if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE 
            || parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT
            || parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT) {

        if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_NEGATE )
            tac_node.type = TAC_NODE_INSTRUCTION_UNARY_NEGATE;
        else if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_BITWISE_NOT) 
            tac_node.type = TAC_NODE_INSTRUCTION_UNARY_BITWISE_NOT;
        else if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_UNOP_LOGICAL_NOT) 
            tac_node.type = TAC_NODE_INSTRUCTION_UNARY_LOGICAL_NOT;
        else 
            ASSERT(0);

        Tac_Node_Operand src = create_tac_instructions( tac_instructions, parse_expression->expression.factor.unop_next);
        Tac_Node_Operand dest = ZERO_STRUCT;
        dest.type =TAC_NODE_OPERAND_VAR; 
        dest.int_value = var_name_index;
        dest.identifier = str_create_from_cstr( var_name);
        str_append_int( &dest.identifier, var_name_index);
        var_name_index += 1;
        tac_node.instruction.unary.src = src;
        tac_node.instruction.unary.dest = dest;
        array_append(tac_instructions, tac_node);
        return dest;
    }else  if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_EXP ){
        return create_tac_instructions( tac_instructions, parse_expression->expression.factor.expression);
    }else  if (parse_expression->type == PARSE_TYPE_EXP_BINOP_ADD 
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_EQUAL
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_SUB
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_MULTIPLY
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_DIVIDE
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_MODULO
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_OR
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_AND
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_XOR
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO
            || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO) {

        if (parse_expression->type == PARSE_TYPE_EXP_BINOP_ADD )
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_ADD;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_EQUAL) 
            tac_node.type = TAC_NODE_INSTRUCTION_COPY;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_SUB) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_SUB;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_MULTIPLY) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_MULTIPLY;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_DIVIDE) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_DIVIDE;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_MODULO) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_MODULO;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_OR) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_AND) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_XOR) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_RIGHT) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_BITWISE_SHIFT_LEFT) 
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_LESS_THAN;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_LESS_THAN_EQUAL_TO)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_GREATER_THAN_EQUAL_TO)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_EQUAL_TO)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO;
        else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_NOT_EQUAL_TO)
            tac_node.type = TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO;
        else
            ASSERT(0);



        Tac_Node_Operand src1 = create_tac_instructions( tac_instructions, parse_expression->expression.binop.left);
        Tac_Node_Operand src2 = create_tac_instructions( tac_instructions, parse_expression->expression.binop.right);

            Tac_Node_Operand dest = ZERO_STRUCT;
        if (tac_node.type == TAC_NODE_INSTRUCTION_COPY) {
            tac_node.instruction.copy.src = src2;
            tac_node.instruction.copy.dest = src1;
            dest = src1;
        } else {
            dest.type =TAC_NODE_OPERAND_VAR; 
            dest.int_value = var_name_index;
            dest.identifier = str_create_from_cstr( var_name);
            str_append_int( &dest.identifier, var_name_index);
            var_name_index += 1;
            tac_node.instruction.binary.src1 = src1;
            tac_node.instruction.binary.src2 = src2;
            tac_node.instruction.binary.dest = dest;
        }
        array_append(tac_instructions, tac_node);
        return dest;

    }else  if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_AND 
               || parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR ) {

        Tac_Node_Operand one = {0};
        one.type = TAC_NODE_OPERAND_INT;
        one.int_value = 1;
        one.identifier = str_create_from_int( 1);

        Tac_Node_Operand zero = {0};
        zero.type = TAC_NODE_OPERAND_INT;
        zero.int_value = 0;
        zero.identifier = str_create_from_int( 0);

        Tac_Node_Operand dest = ZERO_STRUCT;
        dest.type =TAC_NODE_OPERAND_VAR; 
        dest.int_value = var_name_index;
        dest.identifier = str_create_from_cstr( var_name);
        str_append_int( &dest.identifier, var_name_index);
        var_name_index += 1;

        Tac_Node_Operand fail_label_jmp = ZERO_STRUCT;
        fail_label_jmp.type = TAC_NODE_INSTRUCTION_LABEL;
        fail_label_jmp.int_value = label_name_index;
        fail_label_jmp.identifier = str_create_from_cstr( label_name);
        str_append_int( &fail_label_jmp.identifier, label_name_index);
        label_name_index += 1;

        Tac_Node_Operand success_label_jmp = ZERO_STRUCT;
        success_label_jmp.type = TAC_NODE_INSTRUCTION_LABEL;
        success_label_jmp.int_value = label_name_index;
        success_label_jmp.identifier = str_create_from_cstr( label_name);
        str_append_int( &success_label_jmp.identifier, label_name_index);
        label_name_index += 1;

        Tac_Node_Operand src1 = create_tac_instructions( tac_instructions, parse_expression->expression.binop.left);
        Tac_Node jmp1 = ZERO_STRUCT;
        if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR)
            jmp1.type = TAC_NODE_INSTRUCTION_JINZ;
        else 
            jmp1.type = TAC_NODE_INSTRUCTION_JIZ;
        jmp1.instruction.jmp.condition = src1;
        jmp1.instruction.jmp.label = fail_label_jmp;
        array_append(tac_instructions, jmp1);

        Tac_Node_Operand src2 = create_tac_instructions( tac_instructions, parse_expression->expression.binop.right);
        Tac_Node jmp2 = ZERO_STRUCT;
        if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR)
            jmp2.type = TAC_NODE_INSTRUCTION_JINZ;
        else 
            jmp2.type = TAC_NODE_INSTRUCTION_JIZ;



        jmp2.instruction.jmp.condition = src2;
        jmp2.instruction.jmp.label = fail_label_jmp;
        array_append(tac_instructions, jmp2);

        Tac_Node copy_first = ZERO_STRUCT;
        copy_first.type = TAC_NODE_INSTRUCTION_COPY;
        if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_AND) {
            copy_first.instruction.copy.src = one;
        } else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR) {
            copy_first.instruction.copy.src = zero;
        } else
            ASSERT(0);
        copy_first.instruction.copy.dest = dest;
        array_append(tac_instructions, copy_first);

        Tac_Node jmp3 = ZERO_STRUCT;
        jmp3.type = TAC_NODE_INSTRUCTION_JMP;
        jmp3.instruction.jmp.label = success_label_jmp;
        array_append(tac_instructions, jmp3);

        Tac_Node fail_label = ZERO_STRUCT;
        fail_label.type = TAC_NODE_INSTRUCTION_LABEL;
        fail_label.instruction.label.operand = fail_label_jmp;
        array_append(tac_instructions, fail_label);

        Tac_Node copy_second = ZERO_STRUCT;
        copy_second.type = TAC_NODE_INSTRUCTION_COPY;
        if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_AND) {
            copy_second.instruction.copy.src = zero;
        }else if (parse_expression->type == PARSE_TYPE_EXP_BINOP_LOGICAL_OR) {
            copy_second.instruction.copy.src = one;
        } else
            ASSERT(0);
        copy_second.instruction.copy.dest = dest;
        array_append(tac_instructions, copy_second);

        Tac_Node success_label = ZERO_STRUCT;
        success_label.type = TAC_NODE_INSTRUCTION_LABEL;
        success_label.instruction.label.operand = success_label_jmp;
        array_append(tac_instructions, success_label);

        //tac_node.instruction.binary.src1 = src1;
        //tac_node.instruction.binary.src2 = src2;
        //tac_node.instruction.binary.dest = dest;
        //array_append(tac_instructions, tac_node);
        return dest;
    }else if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_INT) {
        Tac_Node_Operand operand = ZERO_STRUCT;
        //if (parse_expression->expression.operand.type == PARSE_TYPE_EXP_OPERAND_INT) {
        operand.type = TAC_NODE_OPERAND_INT;
        operand.int_value = parse_expression->expression.factor.int_value;
        operand.identifier = str_create_from_int( operand.int_value);
        return operand;
        // }else {
        //             FAIL_MSG( "Tac operand fail");
        // }

    }else if (parse_expression->type == PARSE_TYPE_EXP_FACTOR_VAR) {
        Tac_Node_Operand operand = parse_var_to_ir(parse_expression);

        return operand;
        // }else {
        //             FAIL_MSG( "Tac operand fail");
        // }

    }else {
        FAIL_MSG( "Tac instruction fail");
    }


}


Tac_Node_Operand parse_var_to_ir(Parse_Node *parse_var) {
    ASSERT(parse_var->type == PARSE_TYPE_EXP_FACTOR_VAR);
    Tac_Node_Operand operand = ZERO_STRUCT;
    operand.type = TAC_NODE_OPERAND_VAR;
    operand.int_value = parse_var->expression.factor.int_value;
    operand.identifier = str_create_from_cstr( function_var_name);
    str_append_str(&operand.identifier, parse_var->expression.factor.var_name);
    str_append_cstr(&operand.identifier, ".");
    str_append_int( &operand.identifier, operand.int_value);
    return operand;
}

void create_tac_functions( Tac_Node **tac_functions, Parse_Node *parse_function)
{
    Tac_Node tac_function  = ZERO_STRUCT;
    tac_function.type = TAC_NODE_FUNCTION;
    tac_function.function.name = str_clone( parse_function->function.name);
    tac_function.function.return_value_name = str_clone( parse_function->function.return_value_name);
    tac_function.function.name = str_clone( parse_function->function.name);

    int parse_var_count = get_array_count(parse_function->function.var_table.var_items);
    int block_item_count = get_array_count(parse_function->function.block_items);
    for  (int block_item_i = 0; block_item_i < block_item_count; ++block_item_i) {
        Parse_Node *parse_statement = &parse_function->function.block_items[block_item_i];
        if (parse_statement->type == PARSE_TYPE_STATEMENT_RETURN) {
            Parse_Node *parse_expression = parse_statement->statement.expression;
            Tac_Node return_instruction = ZERO_STRUCT;
            return_instruction.type = TAC_NODE_INSTRUCTION_RETURN;
            Tac_Node_Operand return_operand = create_tac_instructions( &tac_function.function.instructions, parse_expression);
            return_instruction.instruction.return_operand = return_operand;
            array_append(&tac_function.function.instructions, return_instruction);
        }else if (parse_statement->type == PARSE_TYPE_STATEMENT_DECLARATION) {
            if (parse_statement->declaration.r_value) {
                Parse_Node *right = parse_statement->declaration.r_value;
                Parse_Node *left = parse_statement->declaration.l_value;
                Tac_Node copy = ZERO_STRUCT;
                copy.type = TAC_NODE_INSTRUCTION_COPY;
                copy.instruction.copy.src = create_tac_instructions( &tac_function.function.instructions, right);
                copy.instruction.copy.dest = parse_var_to_ir(left);

                array_append(&tac_function.function.instructions, copy);
            }
        }else if (parse_statement->type == PARSE_TYPE_STATEMENT_EXPRESSION) {
            create_tac_instructions(&tac_function.function.instructions, parse_statement->statement.expression);


        }else if (parse_statement->type == PARSE_TYPE_STATEMENT_NULL) {
            continue;
        } else {
            ASSERT(0);
        }
    }

    Tac_Node return_instruction = ZERO_STRUCT;
    return_instruction.type = TAC_NODE_INSTRUCTION_RETURN;
    Tac_Node_Operand operand = {0};
    operand.type = TAC_NODE_OPERAND_INT;
    operand.int_value = 0;
    operand.identifier = str_create_from_int(0);
    return_instruction.instruction.return_operand = operand;
    array_append(&tac_function.function.instructions, return_instruction);
    array_append(tac_functions, tac_function);
}

Tac_Node * create_tac_nodes( Parse_Node *parse_program)
{
    Tac_Node tac_program = ZERO_STRUCT;
    tac_program.type = TAC_NODE_PROGRAM;
    ASSERT(parse_program->type == PARSE_TYPE_PROGRAM);
    for (size_t i = 0; i < get_array_count(parse_program->program.functions); ++i) {

        int parse_var_count = get_array_count(parse_program->program.functions[i].function.var_table.var_items);
        var_name_index += parse_var_count;
        create_tac_functions( &tac_program.program.functions, &parse_program->program.functions[i]);
        tac_program.program.functions[i].function.stack_size = var_name_index;
        var_name_index = 0;
        label_name_index = 0;
    }
    Tac_Node *rv = 0;
    array_append(&rv, tac_program);
    return rv;
}
