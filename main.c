#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "tac.h"
#include "parse.h"
#include "lex.h"
#include "code_gen.h"
#include "code_emmission.h"

#include "arena.h"
#include "string.h"
#include "tdf.h"


int main(int argc, char**argv) 
{
    arena_init();
    String input_name = {0};
    String output_name = {0};
    bool output_object_file = false;
    if (argc == 1) {
        input_name = str_create_from_cstr("input.c");
    } else {
        int current_argc = 1;
        bool found_input_name = false;
        for (int i = 1; i <argc; ++i) {
            String_View arg = sv_from_cstr(argv[i]);
            if (sv_equals_cstr(arg, "-c")) {
                output_object_file =true;
                i += 1;
                ASSERT(i < argc);
                input_name =str_create_from_cstr(argv[i]); 
                found_input_name = true;

                i += 1;
                ASSERT(i < argc);
                output_name =str_create_from_cstr(argv[i]); 
                continue;

            }
            if (found_input_name)
                ASSERT(0);
            found_input_name = true;
            input_name = str_create_from_cstr(argv[i]);
        }
    }
    if (output_object_file) {
        printf("Object File Create\n");
        String command = str_create_from_cstr("gcc -c ");
        str_append_str(&command, input_name);
        str_append_cstr(&command, " ");
        str_append_str(&command, output_name);
        char *cstr_command = str_convert_to_cstr(command);
        system(cstr_command);
        arena_deinit();
        return 0;
    }

    String input_dir = ZERO_STRUCT;
    String_View input_name_sv = sv_from_str(input_name);
    String_View input_filename_head  = sv_chop_delim(&input_name_sv, '.');
    str_append_sv( &output_name, input_filename_head);
    str_append_cstr( &output_name, ".s");

    Token *tokens = lex_program( input_name);
    print_tokens(tokens);
    Parse_Node *parse_nodes = parse_tokens( &tokens);
    print_parse_nodes(parse_nodes);
    Tac_Node *tac_nodes = create_tac_nodes( parse_nodes);
    print_tac_nodes(tac_nodes);
    Assembly_Node *assembly_nodes = create_code_gen( tac_nodes);
    printf("\nCode gen\n");
    print_assembly_nodes(assembly_nodes);


    emmit_code( assembly_nodes, output_name);


    free_tokens(tokens);
    free_parse_nodes(parse_nodes);
    free_tac_nodes(tac_nodes) ;
    free_assembly_nodes(assembly_nodes);
    arena_deinit();
    return (0);
}
