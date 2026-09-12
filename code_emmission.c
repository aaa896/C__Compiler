#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"
#include "code_gen.h"


char *label_prefix = ".L";

void emmit_instructions( String *output , Assembly_Node *node)
{
    int instruction_count = get_array_count(node);
    str_append_cstr( output, "    pushq %rbp\n");
    str_append_cstr( output, "    movq %rsp, %rbp\n");
    for (int i = 0; i < instruction_count; ++i) {
        Assembly_Node *instruction = &node[i];
        if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV) {
            str_append_cstr( output, "    movl ");
            str_append_str( output, instruction->mov.src.identifier);
            str_append_cstr( output, " , ");
            str_append_str( output, instruction->mov.dest.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NEGATE) {
            str_append_cstr( output, "    negl ");
            str_append_str( output, instruction->unary.operand.identifier);
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_BITWISE_NOT) {
            str_append_cstr( output, "    notl ");
            str_append_str( output, instruction->unary.operand.identifier);
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_CDQ) {
            str_append_cstr( output, "    cdq\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_IDIV) {
            str_append_cstr( output, "    idiv ");
            str_append_str( output, instruction->idiv.operand.identifier);
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_ADD ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_SUB ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_AND ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_OR ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_XOR ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT ||
                instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT ) {

            if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_ADD )
                str_append_cstr( output, "    addl ");
            else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY )
                str_append_cstr( output, "    imull ");
            else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_SUB ) 
                str_append_cstr( output, "    subl ");
            else if  ( instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_AND )
                str_append_cstr( output, "    andl ");
            else if  ( instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_OR )
                str_append_cstr( output, "    orl ");
            else if  ( instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_XOR )
                str_append_cstr( output, "    xorl ");
            else if  ( instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT )
                str_append_cstr( output, "    sall ");
            else if  ( instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT )
                str_append_cstr( output, "    sarl ");
            else 
                ASSERT(0);

            str_append_str( output, instruction->binary.src.identifier);
            str_append_cstr( output, " , ");
            str_append_str( output, instruction->binary.dest.identifier);
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMP) {
            str_append_cstr( output, "    jmp ");
            str_append_str( output, str_join_cstr_str( label_prefix, instruction->jmp.operand.identifier));
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPE) {
            str_append_cstr( output, "    je ");
            str_append_str( output, str_join_cstr_str( label_prefix, instruction->jmp.operand.identifier));
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPNE) {
            str_append_cstr( output, "    jne ");
            str_append_str( output, str_join_cstr_str( label_prefix, instruction->jmp.operand.identifier));
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETGE) {
            str_append_cstr( output, "    setge ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETG) {
            str_append_cstr( output, "    setg ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETLE) {
            str_append_cstr( output, "    setle ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETL) {
            str_append_cstr( output, "    setl ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETE) {
            str_append_cstr( output, "    sete ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETNE) {
            str_append_cstr( output, "    setne ");
            str_append_str( output,  instruction->set.operand.identifier);
            str_append_cstr( output, "\n");

        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL) {

            str_append_str( output, str_join_cstr_str( label_prefix, instruction->label.operand.identifier));
            str_append_cstr( output, ":");
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP) {
            str_append_cstr( output, "    cmpl ");
            str_append_str( output, instruction->cmp.operand_a.identifier);
            str_append_cstr( output, ",");
            str_append_str( output, instruction->cmp.operand_b.identifier);
            str_append_cstr( output, "\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_ALLOCATE_STACK) {
            str_append_cstr( output, "    subq $");
            str_append_int(  output,  instruction->stack.operand.int_value );
            str_append_cstr( output, " , %rsp\n");
        }else if (instruction->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_RET) {
            str_append_cstr( output, "    movq %rbp, %rsp\n");
            str_append_cstr( output, "    popq %rbp\n");
            str_append_cstr( output, "    ret\n");
        } else{
            FAIL_MSG( "unknown emission print\n");
        }
    }
}

void emmit_function( String *output, Assembly_Node *node)
{
    if (node->type != ASSEMBLY_NODE_TYPE_FUNCTION) {
        FAIL_MSG("code emiission expected function\n");
    }
    str_append_cstr( output, "    .global ");
    str_append_str( output, node->function.name);
    str_append_cstr( output, "\n");
    str_append_str( output, node->function.name);
    str_append_cstr( output, ":\n");
    emmit_instructions( output, node->function.instructions);
     str_append_cstr( output, ".section .note.GNU-stack,\"\",@progbits\n");
}



String emmit_program( Assembly_Node *node)
{
    if (node->type != ASSEMBLY_NODE_TYPE_PROGRAM) {
        FAIL_MSG("emmit error expected assembly node program\n");
    }
    String output = ZERO_STRUCT;
    int function_count = get_array_count(node->program.functions);
    for (int i = 0; i < function_count; ++i) {
        Assembly_Node *function = &node->program.functions[i];
        emmit_function( &output, function);
    }
    return output;
}

void emmit_code( Assembly_Node *node, String path)
{
    String output =  emmit_program( node);
    printf("\nCode emission\n");
    str_print(output);

    char *cstr_path = str_convert_to_cstr( path);
    FILE *file = fopen(cstr_path, "wb");
    fwrite(output.data, 1, output.size, file);
    fclose(file);
}
