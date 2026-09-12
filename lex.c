#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"
#include "dynamic_array.h"
#include "string.h"




void print_tokens(Token *tokens) {
    printf("\nTokens Print\n");
    int array_count = get_array_count(tokens);
    for (int i = 0; i < array_count ; ++i) {
        if (tokens[i].type == TOKEN_TYPE_IDENTIFIER) {
            printf("Identifier ");
            str_print(tokens[i].identifier);
            printf("\n");
        } else if (tokens[i].type == TOKEN_TYPE_NUMBER) {
            printf("Number ");
            str_print(tokens[i].identifier);
            printf(" \n");
        } else if (tokens[i].type == TOKEN_TYPE_OPEN_PAREN) {
            printf("Open Paren (\n");
        } else if (tokens[i].type == TOKEN_TYPE_CLOSE_PAREN) {
            printf("Close Paren )\n");
        } else if (tokens[i].type == TOKEN_TYPE_OPEN_BRACKET) {
            printf("Open Bracket {\n");
        } else if (tokens[i].type == TOKEN_TYPE_CLOSE_BRACKET) {
            printf("Close Bracket }\n");
        } else if (tokens[i].type == TOKEN_TYPE_SEMICOLON) {
            printf("Semicolon ;\n");
        } else if (tokens[i].type == TOKEN_TYPE_DECREMENT) {
            printf("Decrement --\n");
        } else if (tokens[i].type == TOKEN_TYPE_NEGATE) {
            printf("Negate -\n");
        } else if (tokens[i].type == TOKEN_TYPE_TILDE) {
            printf("Tilde ~\n");
        } else if (tokens[i].type == TOKEN_TYPE_PLUS) {
            printf("Plus +\n");
        } else if (tokens[i].type == TOKEN_TYPE_ASTERIX) {
            printf("Asterix *\n");
        } else if (tokens[i].type == TOKEN_TYPE_FORWARD_SLASH) {
            printf("Forward Slash /\n");
        } else if (tokens[i].type == TOKEN_TYPE_PERCENT) {
            printf("Percent %% \n");
        } else if (tokens[i].type == TOKEN_TYPE_AMPERSAND) {
            printf("Ampersand & \n");
        } else if (tokens[i].type == TOKEN_TYPE_VERTICAL_BAR) {
            printf("Vertical Bar | \n");
        } else if (tokens[i].type == TOKEN_TYPE_CARROT) {
            printf("Carrot ^ \n");
        } else if (tokens[i].type == TOKEN_TYPE_LEFT_SHIFT) {
            printf("Left shift <<  \n");
        } else if (tokens[i].type == TOKEN_TYPE_RIGHT_SHIFT) {
            printf("Right shift >> \n");
        } else if (tokens[i].type == TOKEN_TYPE_LESS_THAN) {
            printf("Less than < \n");
        } else if (tokens[i].type == TOKEN_TYPE_LESS_THAN_EQUAL_TO) {
            printf("Less than Equal to <= \n");
        } else if (tokens[i].type == TOKEN_TYPE_EQUAL_TO) {
            printf("Equal to == \n");
        } else if (tokens[i].type == TOKEN_TYPE_GREATER_THAN) {
            printf("Greater than > \n");
        } else if (tokens[i].type == TOKEN_TYPE_GREATER_THAN_EQUAL_TO) {
            printf("Greater than Equal to >= \n");
        } else if (tokens[i].type == TOKEN_TYPE_LOGICAL_OR) {
            printf("Logical or || \n");
        } else if (tokens[i].type == TOKEN_TYPE_LOGICAL_AND) {
            printf("Logical and && \n");
        } else if (tokens[i].type == TOKEN_TYPE_EQUAL_SIGN) {
            printf("Equal sign = \n");
        } else if (tokens[i].type == TOKEN_TYPE_NOT_EQUAL_TO) {
            printf("Not Equal to != \n");
        } else if (tokens[i].type == TOKEN_TYPE_EXCLAMATION) {
            printf("Exclamation ! \n");

        }else if (TOKEN_TYPE_PLUS_EQUAL) {
            printf("Compound Plus +=\n");
        }else if (TOKEN_TYPE_ASTERIX_EQUAL) {
            printf("Compound multiply *= \n");
        }else if (TOKEN_TYPE_FORWARD_SLASH_EQUAL) {
            printf("Compound divide /=\n");
        }else if (TOKEN_TYPE_PERCENT_EQUAL) {
            printf("Compound remainder %%= \n");
        }else if (TOKEN_TYPE_AMPERSAND_EQUAL) {
            printf("Compound And &=\n");
        }else if (TOKEN_TYPE_VERTICAL_BAR_EQUAL) {
            printf("Compound Or |= \n");
        }else if (TOKEN_TYPE_CARROT_EQUAL) {
            printf("Compound Xor ^=\n");
        }else if (TOKEN_TYPE_LEFT_SHIFT_EQUAL) {
            printf("Compound Left shift <<= \n");
        }else if (TOKEN_TYPE_RIGHT_SHIFT_EQUAL) {
            printf("Compound Right shift >>=\n");
        }else {
            ASSERT( 0 && "Uknown print token\n");
        }
    }
}

static int peek_char(String *line, int index, char c) {
    if (index >= line->size)
        return 0;
    return (line->data[index] == c);
}


Token * lex_program( String path) 
{
    Token * tokens = 0;
    String program = str_read_file_from_str( path);
    if (!program.data){
        FAIL_MSG("could not open ");
    }
    str_print(program);
    int col = 0;
    int row = 0;

    for (int i = 0; i < program.size; ++i) {
        if (program.data[i] == ' ' ){
        }else if (program.data[i] == '\t') {
        } else if (program.data[i] == '\n') {
            row +=1;
            col = 0;
        } else if (isalpha(program.data[i])) {
            Token token  = ZERO_STRUCT;
            token.type = TOKEN_TYPE_IDENTIFIER;
            String_View identifier = ZERO_STRUCT;
            identifier.count  = 1;
            identifier.data = &program.data[i];

            for (; identifier.count + i < program.size; ++identifier.count) {
                if (!isalnum(program.data[i + identifier.count])) {
                    break;
                }
            }

            token.identifier = str_create_from_sv( identifier);
            token.col = col;
            token.row = row;
            i   += token.identifier.size -1;
            col += token.identifier.size;
            array_append(&tokens, token);
        } else if (isdigit(program.data[i])) {
            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_NUMBER;
            String_View identifier = ZERO_STRUCT;
            identifier.count  = 1;
            identifier.data = &program.data[i];

            for (; identifier.count + i < program.size; ++identifier.count) {
                if (!isdigit(program.data[i + identifier.count])) {
                    if (isalpha(program.data[i + identifier.count])) {
                        FAIL_MSG("alnum cannot be next to number\n");
                    }
                    break;
                }
            }
            token.identifier = str_create_from_sv( identifier);
            token.col  = col;
            token.row = row;

            i += token.identifier.size -1;
            col += token.identifier.size;
            token.constant = atoi(str_convert_to_cstr( token.identifier));
            array_append(&tokens, token);
        }else if (program.data[i] == '/') {
            if (peek_char(&program, i + 1, '/')) {
                while (i < program.size && !peek_char(&program, i + 1, '\n')) {
                    i +=1;
                }
            }else if (peek_char(&program, i +1, '*')) {
                while(1) {
                    i+=1;
                    if (i == program.size) {
                        FAIL_MSG("/* comment unterminated\n");
                    }
                    if (peek_char(&program, i, '*')) {
                        if (peek_char(&program, i + 1, '/')) {
                            break;
                        }
                    }

                }
            } else {

                Token token = ZERO_STRUCT;
                if (peek_char(&program, i + 1, '=')) {
                    token.type = TOKEN_TYPE_FORWARD_SLASH_EQUAL;
                    i += 1;
                } else {
                    token.type = TOKEN_TYPE_FORWARD_SLASH;
                }
                array_append(&tokens, token);
            }
        }else if (program.data[i] == '~') {
            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_TILDE;
            array_append(&tokens, token);
        }else if (program.data[i] == '%') {
            Token token = ZERO_STRUCT;
                        if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_PERCENT_EQUAL;
                i += 1;
                        } else {
            token.type = TOKEN_TYPE_PERCENT;
                        }
            array_append(&tokens, token);

        }else if (program.data[i] == '*') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_ASTERIX_EQUAL;
                i += 1;
            } else {
                token.type = TOKEN_TYPE_ASTERIX;
            }
            array_append(&tokens, token);

        }else if (program.data[i] == '+') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_PLUS_EQUAL;
                i += 1;
            } else {
            token.type = TOKEN_TYPE_PLUS;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '-') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '-')) {
                token.type = TOKEN_TYPE_DECREMENT;
                i +=1;
            }else {
                token.type = TOKEN_TYPE_NEGATE;
            }
            array_append(&tokens, token);

        }else if (program.data[i] == '!') {

            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_NOT_EQUAL_TO;
                i +=1;
            }else {
                token.type = TOKEN_TYPE_EXCLAMATION;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '=') {

            Token token = ZERO_STRUCT;

            if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_EQUAL_TO;
                i +=1;
            }else {
                token.type = TOKEN_TYPE_EQUAL_SIGN;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '&') {

            Token token = ZERO_STRUCT;

            if (peek_char(&program, i + 1, '&')) {
                token.type = TOKEN_TYPE_LOGICAL_AND;
                i +=1;
            }else if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_AMPERSAND_EQUAL;
                i += 1;
            }else {

                token.type = TOKEN_TYPE_AMPERSAND;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '^') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_CARROT_EQUAL;
                i += 1;
            } else {
                token.type = TOKEN_TYPE_CARROT;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '|') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '|')) {
                token.type = TOKEN_TYPE_LOGICAL_OR;
                i +=1;
            }if (peek_char(&program, i + 1, '=')) {
                token.type = TOKEN_TYPE_VERTICAL_BAR_EQUAL;
                i += 1;
            }else {
                token.type = TOKEN_TYPE_VERTICAL_BAR;
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '>') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '>')) {
                token.type = TOKEN_TYPE_RIGHT_SHIFT;
                i +=1;
                if (peek_char(&program, i + 1, '=')) {
                    token.type = TOKEN_TYPE_RIGHT_SHIFT_EQUAL;
                    i += 1;
                }

            }else {
                if (peek_char(&program, i + 1, '=')) {
                    token.type = TOKEN_TYPE_GREATER_THAN_EQUAL_TO;
                    i +=1;
                }else {
                    token.type = TOKEN_TYPE_GREATER_THAN;
                }
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '<') {
            Token token = ZERO_STRUCT;
            if (peek_char(&program, i + 1, '<')) {
                token.type = TOKEN_TYPE_LEFT_SHIFT;
                i +=1;
                if (peek_char(&program, i + 1, '=')) {
                    token.type = TOKEN_TYPE_LEFT_SHIFT_EQUAL;
                    i += 1;
                }

            }else {
                if (peek_char(&program, i + 1, '=')) {
                    token.type = TOKEN_TYPE_LESS_THAN_EQUAL_TO;
                    i +=1;
                }else {
                    token.type = TOKEN_TYPE_LESS_THAN;
                }
            }
            array_append(&tokens, token);
        }else if (program.data[i] == '(') {
            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_OPEN_PAREN;
            array_append(&tokens, token);
        }else if (program.data[i] == ')') {

            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_CLOSE_PAREN;
            array_append(&tokens, token);

        }else if (program.data[i] == '{') {
            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_OPEN_BRACKET;
            array_append(&tokens, token);
        }else if (program.data[i] == '}') {

            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_CLOSE_BRACKET;
            array_append(&tokens, token);
        }else if (program.data[i] == ';') {
            Token token = ZERO_STRUCT;
            token.type = TOKEN_TYPE_SEMICOLON;
            array_append(&tokens, token);
        }else  {
            FAIL_MSG( "Unhandled unknown token\n");
        }


        col += 1;
    }

    return tokens;

}
