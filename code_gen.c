#include <stdio.h>
#include <stdlib.h>

#include "code_gen.h"
#include "tac.h"
#include "parse.h"

#include "string.h"
#include "dynamic_array.h"


static void print_char_n(char c, int n) 
{
    for (int i = 0; i < n; ++i) {
        putchar(c);
    }
}

void print_operand(Assembly_Node_Operand *operand) 
{
    if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_INT) {
        printf("%d", operand->int_value);
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {
        //printf("stack ");
        str_print(operand->identifier);
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_EAX) {
        printf("eax");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_EDX) {
        printf("edx");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_ECX) {
        printf("ecx");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_CL) {
        printf("cl");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_R10) {
        printf("r10");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_OPERAND_REG_R11) {
        printf("r11");
    }else if (operand->type == ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL) {
        str_print(operand->identifier);
    }else {FAIL_MSG( "print operand invalid assembly_node");}
}

void print_assembly_nodes_pad(Assembly_Node *nodes, int start_padding, int padding_increment) 
{
    if (!nodes)
        return;
    for (size_t i =0; i < get_array_count(nodes); ++i) {
        print_char_n(' ', start_padding);
        if (nodes[i].type == ASSEMBLY_NODE_TYPE_FUNCTION) {
            printf("Function ");
            str_print(nodes[i].function.name);
            printf(" {\n");
            print_assembly_nodes_pad(nodes[i].function.instructions, start_padding + padding_increment, padding_increment);
            print_char_n(' ', start_padding);
            printf("}\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_PROGRAM) {
            printf("Program {\n");
            print_assembly_nodes_pad(nodes[i].program.functions, start_padding + padding_increment, padding_increment);
            print_char_n(' ', start_padding);
            printf("}\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_ALLOCATE_STACK) {
            printf("alloc stack(");
            print_operand(&nodes[i].stack.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV) {
            printf("mov (");
            print_operand(&nodes[i].mov.src);
            printf(", ");
            print_operand(&nodes[i].mov.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_ADD) {
            printf("add (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY) {
            printf("mull  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_SUB) {
            printf("sub  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_AND) {
            printf("andl  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_OR) {
            printf("orl  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_XOR) {
            printf("xorl  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT) {
            printf("sall (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT) {
            printf("sarl (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT) {
            printf("sarl  (");
            print_operand(&nodes[i].binary.src);
            printf(",");
            print_operand(&nodes[i].binary.dest);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_CDQ) {
            printf("CDQ \n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_IDIV) {
            printf("idiv / (");
            print_operand(&nodes[i].idiv.operand);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_BITWISE_NOT) {
            printf("unary bitwise ~ (");
            print_operand(&nodes[i].unary.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NEGATE) {
            printf("unary negate -(");
            print_operand(&nodes[i].unary.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NOT) {
            printf("unary not !(");
            print_operand(&nodes[i].unary.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP) {
            printf("cmp(");
            print_operand(&nodes[i].cmp.operand_a);
            printf(",");
            print_operand(&nodes[i].cmp.operand_b);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETNE) {
            printf("setne   (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETE) {
            printf("sete   (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETGE) {
            printf("setge   (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETG) {
            printf("setg   (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETLE) {
            printf("setle (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_SETL) {
            printf("setl   (");
            print_operand(&nodes[i].set.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPE) {
            printf("jmpe   (");
            print_operand(&nodes[i].jmp.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMP) {
            printf("jmp   (");
            print_operand(&nodes[i].jmp.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPNE) {
            printf("jmpne   (");
            print_operand(&nodes[i].jmp.operand);
            printf(")\n");
        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL) {
            printf("Label   (");
            print_operand(&nodes[i].jmp.operand);
            printf(")\n");

        } else if (nodes[i].type == ASSEMBLY_NODE_TYPE_INSTRUCTION_RET) {
            printf("ret \n");
        } else {
            FAIL_MSG( "code gen uknown print\n");
        }
    }
}


void print_assembly_nodes(Assembly_Node *nodes)
{
    int start_padding = 0;
    int padding_increment = 4;
    print_assembly_nodes_pad(nodes, start_padding, padding_increment);
}


Assembly_Node_Operand process_tac_operand(Tac_Node_Operand *operand)
{
    Assembly_Node_Operand rv = ZERO_STRUCT;
    if (operand->type == TAC_NODE_OPERAND_INT) {
        rv.type = ASSEMBLY_NODE_TYPE_OPERAND_INT;
        rv.int_value =  operand->int_value;
        rv.identifier = str_join_cstr_str( "$", str_create_from_int( rv.int_value));
    }else if (operand->type == TAC_NODE_OPERAND_VAR) {
        rv.type = ASSEMBLY_NODE_TYPE_OPERAND_STACK;
        rv.int_value = (operand->int_value + 1) * -4;
        rv.identifier = str_join_str_cstr( str_create_from_int( rv.int_value), "(%rbp)");

    }else if (operand->type == TAC_NODE_INSTRUCTION_LABEL) {
        rv.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL;
        rv.int_value = operand->int_value ;
        rv.identifier =operand->identifier;
    }else {
        FAIL_MSG( "Tac operand unknown in code gen");
    }
    return rv;

}

static void process_binary( Assembly_Node **instructions, Assembly_Node binary)
{
    ASSERT(binary.binary.dest.type != ASSEMBLY_NODE_TYPE_OPERAND_INT);
    if (binary.binary.dest.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK &&
            binary.binary.src.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {
        Assembly_Node mov_1 = ZERO_STRUCT;
        mov_1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
        mov_1.mov.src = binary.binary.src;
        mov_1.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        mov_1.mov.dest.identifier = str_create_from_cstr( "%r10d");

        Assembly_Node binary_2 = ZERO_STRUCT;
        binary_2.type = binary.type;
        binary_2.binary.src.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        binary_2.binary.src.identifier = str_create_from_cstr( "%r10d");
        binary_2.binary.dest = binary.binary.dest;
        if (binary_2.type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY &&
                binary_2.binary.dest.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {

            Assembly_Node mov_2 = ZERO_STRUCT;
            mov_2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov_2.mov.src = binary_2.binary.dest;
            mov_2.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R11;
            mov_2.mov.dest.identifier = str_create_from_cstr( "%r11d");

            Assembly_Node binary_3 = ZERO_STRUCT;
            binary_3.type = binary_2.type;
            binary_3.binary.src = binary_2.binary.src;
            binary_3.binary.dest = mov_2.mov.dest;


            Assembly_Node mov_4 = ZERO_STRUCT;
            mov_4.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov_4.mov.src = binary_3.binary.dest;
            mov_4.mov.dest = binary_2.binary.dest;

            array_append(instructions, mov_1);
            array_append(instructions, mov_2);
            array_append(instructions, binary_3);
            array_append(instructions, mov_4);
        }else {
            array_append(instructions, mov_1);
            array_append(instructions, binary_2);
        }

    }else {
        if (binary.type == ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY &&
                binary.binary.dest.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {

            Assembly_Node mov_1 = ZERO_STRUCT;
            mov_1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov_1.mov.src = binary.binary.dest;
            mov_1.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R11;
            mov_1.mov.dest.identifier = str_create_from_cstr( "%r11d");

            Assembly_Node binary_2 = ZERO_STRUCT;
            binary_2.type = binary.type;
            binary_2.binary.src = binary.binary.src;
            binary_2.binary.dest = mov_1.mov.dest;


            Assembly_Node mov_3 = ZERO_STRUCT;
            mov_3.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov_3.mov.src = binary_2.binary.dest;
            mov_3.mov.dest = binary.binary.dest;

            array_append(instructions, mov_1);
            array_append(instructions, binary_2);
            array_append(instructions, mov_3);
        }else {
            array_append(instructions, binary);
        }
    }
}


static void process_mov( Assembly_Node **instructions, Assembly_Node mov)
{
    if (mov.mov.dest.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK &&
            mov.mov.src.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {
        Assembly_Node mov_1 = ZERO_STRUCT;
        mov_1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
        mov_1.mov.src = mov.mov.src;
        mov_1.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        mov_1.mov.dest.identifier = str_create_from_cstr( "%r10d");
        Assembly_Node mov_2 = ZERO_STRUCT;
        mov_2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
        mov_2.mov.src.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        mov_2.mov.src.identifier = str_create_from_cstr( "%r10d");
        mov_2.mov.dest = mov.mov.dest;
        array_append(instructions, mov_1);
        array_append(instructions, mov_2);

    }else {
        array_append(instructions, mov);
    }
}

static void process_cmp( Assembly_Node **instructions, Assembly_Node cmp)
{
    if (cmp.cmp.operand_a.type == ASSEMBLY_NODE_TYPE_OPERAND_INT) {
        Assembly_Node_Operand r11 = ZERO_STRUCT;
        r11.identifier = str_create_from_cstr( "%r11d");
        r11.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R11;
        Assembly_Node mov = ZERO_STRUCT;
        mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;

        mov.mov.src = cmp.cmp.operand_a;
        mov.mov.dest = r11;
        cmp.cmp.operand_a = r11;

        array_append(instructions, mov);
    }

    if (cmp.cmp.operand_a.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK &&
            cmp.cmp.operand_b.type == ASSEMBLY_NODE_TYPE_OPERAND_STACK) {
        Assembly_Node mov_1 = ZERO_STRUCT;
        mov_1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
        mov_1.mov.src = cmp.cmp.operand_a;
        mov_1.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        mov_1.mov.dest.identifier = str_create_from_cstr( "%r10d");
        Assembly_Node cmp_2 = ZERO_STRUCT;
        cmp_2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP;
        cmp_2.cmp.operand_a = mov_1.mov.dest;
        cmp_2.cmp.operand_b = cmp.cmp.operand_b;
        array_append(instructions, mov_1);
        array_append(instructions, cmp_2);

    }else {
        array_append(instructions, cmp);
    }
}

static void process_idiv( Assembly_Node **instructions, Assembly_Node idiv)
{
    if (idiv.idiv.operand.type == ASSEMBLY_NODE_TYPE_OPERAND_INT ) {
        Assembly_Node mov_1 = ZERO_STRUCT;

        mov_1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
        mov_1.mov.src = idiv.idiv.operand;
        mov_1.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10;
        mov_1.mov.dest.identifier = str_create_from_cstr( "%r10d");

        Assembly_Node idiv_2 = ZERO_STRUCT;
        idiv_2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_IDIV;
        idiv_2.idiv.operand = mov_1.mov.dest;


        array_append(instructions, mov_1);
        array_append(instructions, idiv_2);

    }else {
        array_append(instructions, idiv);
    }
}

static void process_set( Assembly_Node **instructions, Assembly_Node set)
{
    if (set.type == ASSEMBLY_NODE_TYPE_OPERAND_REG_EAX)
        set.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_AL;
    else if (set.type == ASSEMBLY_NODE_TYPE_OPERAND_REG_EDX)
        set.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_DL;
    else if (set.type == ASSEMBLY_NODE_TYPE_OPERAND_REG_R10)
        set.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R10B;
    else if (set.type == ASSEMBLY_NODE_TYPE_OPERAND_REG_R11)
        set.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_R11B;
    array_append(instructions, set);
}

static void process_jmp( Assembly_Node **instructions, Assembly_Node jmp)
{
    array_append(instructions, jmp);
}


static void process_label( Assembly_Node **instructions, Assembly_Node label)
{
    array_append(instructions, label);
}

static void search_instruction( Assembly_Node **instructions, Tac_Node *tac_instructions) 
{
    int tac_instruction_count = get_array_count(tac_instructions) ;
    for (int i = 0; i < tac_instruction_count; ++i) {
        Tac_Node *instruction = &tac_instructions[i];
        if (instruction->type == TAC_NODE_INSTRUCTION_RETURN ){
            Tac_Node_Operand *operand = &instruction->instruction.return_operand;

            Assembly_Node mov = ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = process_tac_operand( operand);

            mov.mov.dest.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_EAX;
            mov.mov.dest.identifier = str_create_from_cstr( "\%eax");
            array_append(instructions, mov);

            Assembly_Node ret = ZERO_STRUCT;
            ret.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_RET;
            array_append(instructions, ret);

        }else if (instruction->type == TAC_NODE_INSTRUCTION_COPY ){
            Assembly_Node mov = ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = process_tac_operand( &instruction->instruction.copy.src);
            mov.mov.dest = process_tac_operand( &instruction->instruction.copy.dest);
            process_mov( instructions, mov);
        }else if (instruction->type == TAC_NODE_INSTRUCTION_UNARY_NEGATE 
                || instruction->type == TAC_NODE_INSTRUCTION_UNARY_BITWISE_NOT) {
            Tac_Node_Operand *tac_src  = &instruction->instruction.unary.src;
            Tac_Node_Operand *tac_dest = &instruction->instruction.unary.dest;
            Assembly_Node mov = ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = process_tac_operand( tac_src);
            mov.mov.dest = process_tac_operand( tac_dest);
            Assembly_Node unary = ZERO_STRUCT;
            if (instruction->type == TAC_NODE_INSTRUCTION_UNARY_NEGATE )
                unary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_NEGATE;
            else if (instruction->type == TAC_NODE_INSTRUCTION_UNARY_BITWISE_NOT )
                unary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_UNARY_BITWISE_NOT;
            unary.unary.operand = mov.mov.dest;
            process_mov( instructions, mov);
            array_append(instructions, unary);

        }else if (instruction->type == TAC_NODE_INSTRUCTION_UNARY_LOGICAL_NOT ) {
            Assembly_Node_Operand zero = ZERO_STRUCT;
            zero.type = ASSEMBLY_NODE_TYPE_OPERAND_INT;
            zero.int_value = 0;
            zero.identifier = str_create_from_cstr( "$");
            str_append_int( &zero.identifier, zero.int_value);

            Assembly_Node cmp = ZERO_STRUCT;
            cmp.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP;
            cmp.cmp.operand_a = zero;
            cmp.cmp.operand_b = process_tac_operand( &instruction->instruction.unary.src);

            Assembly_Node mov = ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = zero;
            mov.mov.dest = process_tac_operand( &instruction->instruction.unary.dest);

            Assembly_Node set = ZERO_STRUCT;
            set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETE;
            set.set.operand = mov.mov.dest;

            process_cmp( instructions, cmp);
            process_mov( instructions, mov);
            process_set( instructions, set);
        }else if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_ADD 
                || instruction->type == TAC_NODE_INSTRUCTION_BINARY_SUB
                || instruction->type == TAC_NODE_INSTRUCTION_BINARY_MULTIPLY
                || instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND
                || instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR
                || instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR) {

            Tac_Node_Operand *src1 = &instruction->instruction.binary.src1;
            Tac_Node_Operand *src2 = &instruction->instruction.binary.src2;
            Tac_Node_Operand *dest = &instruction->instruction.binary.dest;
            Assembly_Node mov = ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = process_tac_operand( src1);
            mov.mov.dest = process_tac_operand( dest);

            Assembly_Node binary = ZERO_STRUCT;

            if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_ADD )
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_ADD;
            else if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_SUB)
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_SUB;
            else if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_MULTIPLY) 
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_MULTIPLY;
            else if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_AND) 
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_AND;
            else if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_OR) 
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_OR;
            else if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_XOR) 
                binary.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_XOR;
            else
                ASSERT(0);
            binary.binary.src = process_tac_operand( src2);
            binary.binary.dest = mov.mov.dest;


            process_mov( instructions, mov);
            process_binary( instructions, binary);


        }else if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT ||
                instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT ) {
            Tac_Node_Operand *src1 = &instruction->instruction.binary.src1;
            Tac_Node_Operand *src2 = &instruction->instruction.binary.src2;
            Tac_Node_Operand *dest = &instruction->instruction.binary.dest;
            Assembly_Node_Operand ecx = ZERO_STRUCT;
            ecx.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_ECX;
            ecx.identifier = str_create_from_cstr( "\%ecx");
            Assembly_Node_Operand cx = ZERO_STRUCT;
            cx.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_CL;
            cx.identifier = str_create_from_cstr( "\%cl");

            Assembly_Node mov1 = ZERO_STRUCT;
            mov1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov1.mov.src = process_tac_operand( src1);
            mov1.mov.dest = process_tac_operand( dest);

            Assembly_Node mov2 = ZERO_STRUCT;
            mov2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov2.mov.src = process_tac_operand( src2);
            mov2.mov.dest = ecx;

            Assembly_Node shift  = ZERO_STRUCT;

            if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT )
                shift.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_RIGHT;
            else if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT )
                shift.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_BINARY_BITWISE_SHIFT_LEFT;
            else
                ASSERT(0);
            shift.binary.src = cx;
            shift.binary.dest = mov1.mov.dest;


            process_mov( instructions, mov1);
            process_mov( instructions, mov2);
            array_append(instructions, shift);

        }else if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_DIVIDE ||
                instruction->type == TAC_NODE_INSTRUCTION_BINARY_MODULO) {
            Tac_Node_Operand *src1 = &instruction->instruction.binary.src1;
            Tac_Node_Operand *src2 = &instruction->instruction.binary.src2;
            Tac_Node_Operand *dest = &instruction->instruction.binary.dest;

            Assembly_Node_Operand reg_eax = ZERO_STRUCT;
            reg_eax.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_EAX;
            reg_eax.identifier = str_create_from_cstr("%eax");
            Assembly_Node_Operand reg_edx = ZERO_STRUCT;
            reg_edx.type = ASSEMBLY_NODE_TYPE_OPERAND_REG_EDX;
            reg_edx.identifier = str_create_from_cstr("%edx");

            Assembly_Node mov1 = ZERO_STRUCT;
            mov1.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov1.mov.src = process_tac_operand( src1);
            mov1.mov.dest = reg_eax;

            Assembly_Node cdq = ZERO_STRUCT;
            cdq.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_CDQ;

            Assembly_Node idiv = ZERO_STRUCT;
            idiv.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_IDIV;
            idiv.idiv.operand = process_tac_operand( src2);

            Assembly_Node mov2 = ZERO_STRUCT;
            mov2.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            if ( instruction->type == TAC_NODE_INSTRUCTION_BINARY_DIVIDE )
                mov2.mov.src = reg_eax;
            else 
                mov2.mov.src = reg_edx;
            mov2.mov.dest = process_tac_operand( dest);

            process_mov( instructions, mov1);
            array_append(instructions, cdq);
            process_idiv( instructions, idiv);
            process_mov( instructions, mov2);


        }else if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN||
                instruction->type ==TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO||
                instruction->type ==TAC_NODE_INSTRUCTION_BINARY_LESS_THAN||
                instruction->type ==TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO||
                instruction->type ==TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO||
                instruction->type ==TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO) {
            Assembly_Node_Operand zero = ZERO_STRUCT;
            zero.type = ASSEMBLY_NODE_TYPE_OPERAND_INT;
            zero.int_value = 0;
                        zero.identifier = str_create_from_cstr( "$");
            str_append_int( &zero.identifier, zero.int_value);


            Assembly_Node cmp = ZERO_STRUCT;
            cmp.type  = ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP;
            cmp.cmp.operand_a = process_tac_operand( &instruction->instruction.binary.src1);
            cmp.cmp.operand_b = process_tac_operand( &instruction->instruction.binary.src2);

            Assembly_Node mov =ZERO_STRUCT;
            mov.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_MOV;
            mov.mov.src = zero;
            mov.mov.dest = process_tac_operand( &instruction->instruction.binary.dest);

            Assembly_Node set = ZERO_STRUCT;
            if (instruction->type == TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN)
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETG;
            else if (instruction->type ==TAC_NODE_INSTRUCTION_BINARY_GREATER_THAN_EQUAL_TO)
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETGE;
            else if (instruction->type ==TAC_NODE_INSTRUCTION_BINARY_LESS_THAN)
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETL;
            else if (instruction->type ==TAC_NODE_INSTRUCTION_BINARY_LESS_THAN_EQUAL_TO)
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETLE;
            else if (instruction->type ==TAC_NODE_INSTRUCTION_BINARY_EQUAL_TO)
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETE;
            else if ( instruction->type ==TAC_NODE_INSTRUCTION_BINARY_NOT_EQUAL_TO) 
                set.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_SETNE;
            else 
                ASSERT(0);
            set.set.operand = mov.mov.dest;

            process_cmp( instructions, cmp);
            process_mov( instructions, mov);
            process_set( instructions, set);

        }else if (instruction->type == TAC_NODE_INSTRUCTION_JIZ ||
                instruction->type == TAC_NODE_INSTRUCTION_JINZ) {
            Assembly_Node cmp = ZERO_STRUCT;
            cmp.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_CMP;
            Assembly_Node_Operand zero = ZERO_STRUCT;
            zero.type = ASSEMBLY_NODE_TYPE_OPERAND_INT;
            zero.int_value = 0;
                        zero.identifier = str_create_from_cstr( "$");
            str_append_int( &zero.identifier, zero.int_value);


            Tac_Node_Operand condition = instruction->instruction.jmp.condition;
            cmp.cmp.operand_a =zero;
            cmp.cmp.operand_b = process_tac_operand( &condition);


            Assembly_Node jmp = ZERO_STRUCT; 
            if( instruction->type == TAC_NODE_INSTRUCTION_JIZ )
                jmp.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPE;
            else 
                jmp.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_JMPNE;
            jmp.jmp.operand = process_tac_operand( &instruction->instruction.jmp.label) ;

            process_cmp( instructions,cmp);
            process_jmp( instructions,jmp);

        }else if (instruction->type == TAC_NODE_INSTRUCTION_LABEL ) {
            Assembly_Node label = ZERO_STRUCT; 
            label.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_LABEL;
            label.label.operand = process_tac_operand( &instruction->instruction.label.operand) ;
            process_label( instructions, label);
        }else if (instruction->type == TAC_NODE_INSTRUCTION_JMP ) {

            Assembly_Node jmp = ZERO_STRUCT; 
            jmp.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_JMP;
            jmp.jmp.operand = process_tac_operand( &instruction->instruction.jmp.label) ;
            process_jmp( instructions, jmp);
        }else{
            ASSERT( 0 && "code gen uknown Tac statement type \n");
        }

    }
}

static void search_function( Assembly_Node **functions, Tac_Node *tac_function) 
{
    if (tac_function->type != TAC_NODE_FUNCTION) {
        FAIL_MSG("code gen expected Tac type function\n");
    }

    Assembly_Node function = ZERO_STRUCT;
    function.type = ASSEMBLY_NODE_TYPE_FUNCTION;
    function.function.name = str_clone( tac_function->function.name);
    Assembly_Node stack = ZERO_STRUCT;
    stack.type = ASSEMBLY_NODE_TYPE_INSTRUCTION_ALLOCATE_STACK;
    stack.stack.operand.type = ASSEMBLY_NODE_TYPE_OPERAND_INT;
    stack.stack.operand.int_value = tac_function->function.stack_size * 4;
    stack.stack.operand.identifier = str_create_from_int( stack.stack.operand.int_value);
    array_append(&function.function.instructions, stack);
    search_instruction( &function.function.instructions, tac_function->function.instructions);
    array_append(functions, function);
}


Assembly_Node * create_code_gen( Tac_Node *tac_node) 
{
    Assembly_Node program = ZERO_STRUCT;
    program.type = ASSEMBLY_NODE_TYPE_PROGRAM;
    if (tac_node->type != TAC_NODE_PROGRAM) {
        FAIL_MSG( "Code gen expected Tac program type\n");
    }
    int tac_function_count = get_array_count(tac_node->program.functions);
    for (int i = 0; i < tac_function_count; ++i) {
        search_function( &program.program.functions, &tac_node->program.functions[i]);
    }

    Assembly_Node *rv = 0;
    array_append(&rv, program);
    return rv;
}
