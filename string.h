#ifndef _STRING_H_
#define _STRING_H_
#include "tdf.h"

#define SV_FMT "%.*s"
#define SV_ARG(x) x.count,x.data

typedef struct String_View String_View;
struct String_View {
    char *data;
    int count;
};

typedef struct String String;
struct String  {
    char *data;
    int size;
    int capacity;
};

#define STRING_DEFAULT_CAPACITY 16

String  str_create_from_cstr( char *cstr) ;
String  str_create_from_sv( String_View sv) ;
String  str_clone( String str) ;
String  str_read_file_from_cstr( char *path);
String  str_read_file_from_str( String  str);
b32     str_equals_cstr(String sv , char *s);
b32     str_equals_str(String s1, String s2);
String  str_join_str_str( String s1, String s2);
String  str_join_str_cstr( String s1, char *s2);
String  str_join_cstr_str( char *cstr, String s1);
void    str_resize_capacity(String *s1, size_t cap);
void    str_append_str( String *s1, String s2);
void    str_append_cstr( String *s1, char *cstr);
void    str_append_char( String *s1, char c);
void    str_append_sv( String *s1, String_View sv);
void    str_append_int( String *s1, int x);
void    str_append_float( String *s1, float x);
char*   str_convert_to_cstr(String str) ;
String  str_create_from_int(int x);
char*   cstr_create_from_int(int x);
void    str_free(String *string) ;


char *       sv_convert_to_cstr(String_View sv);
void         sv_chop_left(String_View *sv, int n) ;
void         sv_chop_right(String_View *sv,int n) ;
void         sv_trim(String_View *sv) ;
b32          sv_equals_cstr(String_View sv , char *s);
int          sv_to_int(String_View sv);
String_View  sv_chop_type(String_View *sv, int (*istype)(int c)) ;
String_View  sv_chop_leading_substr(String_View *sv, char *substr) ;
String_View  sv_chop_delim(String_View *sv, char c) ;
String_View  sv_from_str(String str);
void str_print(String s) ;
void sv_print(String_View sv) ;

#endif // _STRING_H_
