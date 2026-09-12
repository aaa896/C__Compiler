#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "tac.h"
#include "parse.h"
#include "lex.h"
#include "code_gen.h"
#include "code_emmission.h"

#include "string.h"
#include "tdf.h"


int main(int argc, char**argv) 
{

    String input_name;
    if (argc == 1) {
        input_name = str_create_from_cstr("input.c");
    } else {
        input_name = str_create_from_cstr(argv[1]);
    }

    String input_dir = ZERO_STRUCT;
    String_View input_name_sv = sv_from_str(input_name);
    String_View input_filename_head  = sv_chop_delim(&input_name_sv, '.');
    String output_name = ZERO_STRUCT;
    str_append_sv( &output_name, input_filename_head);
    str_append_cstr( &output_name, ".s");
    String full_output_path  = str_join_str_str( input_dir, output_name);

    Token *tokens = lex_program( input_name);
    print_tokens(tokens);
    Parse_Node *parse_nodes = parse_tokens( tokens);
    print_parse_nodes(parse_nodes);
    Tac_Node *tac_nodes = create_tac_nodes( parse_nodes);
    print_tac_nodes(tac_nodes);
    Assembly_Node *assembly_nodes = create_code_gen( tac_nodes);
    printf("\nCode gen\n");
    print_assembly_nodes(assembly_nodes);


    emmit_code( assembly_nodes, full_output_path);

    return (0);
}
