gcc -g -fsanitize=address  -DSTRING_USE_ARENA -Wall -Wno-write-strings -Wno-unused-variable main.c arena.c tac.c parse.c  lex.c code_gen.c code_emmission.c string.c  -o a  && ./a 
echo "b"
