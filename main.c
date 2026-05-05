#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

constexpr auto g_str_size = 64;
constexpr auto g_value_list_size = 1024;
constexpr auto g_defer_list_size = 32;
constexpr auto g_value_args = 16;
constexpr auto g_slot_list_size = g_value_args;
constexpr auto g_type_list_size = g_value_args;
constexpr auto g_str_list_size = g_value_args;
constexpr auto g_code_size = 65536;
constexpr auto g_operator_precedences = 8;
constexpr auto g_module_stack_size = 8;

typedef enum
{
    g_precedence_arithmetic_0,
    g_precedence_arithmetic_1,
    g_precedence_shift,
    g_precedence_relational_0,
    g_precedence_relational_1,
    g_precedence_bitwise_and,
    g_precedence_bitwise_xor,
    g_precedence_bitwise_or,
    g_precedence_assignment,
    g_precedence_count,
}
precedence_t;

typedef struct
{
    int begin[g_slot_list_size];
    int size;
}
slot_list_t;

typedef enum : int
{
    g_scope_function,
    g_scope_while,
    g_scope_brace,
    g_scope_if,
}
scope_t;

typedef struct
{
    scope_t scope;
    int at;
    int block;
}
defer_t;

typedef struct
{
    defer_t begin[g_defer_list_size];
    int size;
}
defer_list_t;

typedef struct
{
    char begin[g_str_size];
    int size;
}
str_t;

typedef struct
{
    str_t name;
    int stars;
}
type_t;

typedef struct
{
    type_t begin[g_type_list_size];
    int size;
}
type_list_t;

typedef struct
{
    str_t begin[g_str_list_size];
    int size;
}
str_list_t;

typedef struct
{
    type_t type;
    str_t name;
    type_list_t types;
    str_list_t names;
    str_list_t init;
    int slot;
    bool is_lvalue;
    bool unlock_args;
    bool is_function;
}
value_t;

typedef struct
{
    value_t begin[g_value_list_size];
    int size;
}
value_list_t;

typedef struct
{
    char begin[g_code_size];
    int size;
    int at;
    int line;
    str_t path;
}
code_t;

struct
{
    code_t code[g_module_stack_size];
    defer_list_t defers;
    slot_list_t loop_again;
    slot_list_t loop_end;
    value_list_t values;
    value_list_t types;
    int block;
    int slot;
    int tabs;
    int label;
    int module;
}
g_file;

typedef struct
{
    int if_label;
    int else_label;
    int end_label;
    int block;
}
branch_t;

char* const g_escape_alert                     = "\a";
char* const g_escape_backspace                 = "\b";
char* const g_escape_form_feed                 = "\f";
char* const g_escape_newline                   = "\n";
char* const g_escape_carriage_return           = "\r";
char* const g_escape_tab                       = "\t";
char* const g_escape_vertical_tab              = "\v";
char* const g_escape_backslash                 = "\\";
char* const g_escape_apostrophe                = "\'";
char* const g_escape_quotation                 = "\"";
char* const g_escape_question_mark             = "\?";
char* const g_escape_null                      = "\0";
char* const g_llvm_escape_alert                = "\\07";
char* const g_llvm_escape_backspace            = "\\08";
char* const g_llvm_escape_tab                  = "\\09";
char* const g_llvm_escape_newline              = "\\0A";
char* const g_llvm_escape_vertical_tab         = "\\0B";
char* const g_llvm_escape_form_feed            = "\\0C";
char* const g_llvm_escape_carriage_return      = "\\0D";
char* const g_llvm_escape_quotation            = "\\22";
char* const g_llvm_escape_null                 = "\\00";
char* const g_llvm_escape_backslash            = "\\5C";
char* const g_llvm_escape_question_mark        = "\\3F";
char* const g_apostrophe                       = "'";
char* const g_str                              = "%s";
char* const g_red                              = "\033[31m";
char* const g_green                            = "\033[1;32m";
char* const g_white                            = "\033[1;37m";
char* const g_normal                           = "\033[0m";
char* const g_null                             = "null";
char* const g_underscore                       = "_";
char* const g_left_square                      = "[";
char* const g_rite_square                      = "]";
char* const g_left_curl                        = "{";
char* const g_rite_curl                        = "}";
char* const g_left_paren                       = "(";
char* const g_rite_paren                       = ")";
char* const g_semicolon                        = ";";
char* const g_space                            = " ";
char* const g_lower_begin                      = "a";
char* const g_lower_end                        = "z";
char* const g_upper_begin                      = "A";
char* const g_upper_end                        = "Z";
char* const g_digit_begin                      = "0";
char* const g_digit_end                        = "9";
char* const g_not                              = "!";
char* const g_type_cast                        = "<>";
char* const g_function                         = "()";
char* const g_index                            = "[]";
char* const g_dot                              = ".";
char* const g_add                              = "+";
char* const g_subtract                         = "-";
char* const g_divide                           = "/";
char* const g_mod                              = "%";
char* const g_multiply                         = "*";
char* const g_equals                           = "=";
char* const g_equal_to                         = "==";
char* const g_not_equal_to                     = "!=";
char* const g_less_equal_to                    = "<=";
char* const g_less                             = "<";
char* const g_greater_equal_to                 = ">=";
char* const g_greater                          = ">";
char* const g_shift_rite                       = ">>";
char* const g_shift_left                       = "<<";
char* const g_increment                        = "++";
char* const g_decrement                        = "--";
char* const g_bitwise_or                       = "|";
char* const g_bitwise_xor                      = "^";
char* const g_bitwise_and                      = "&";
char* const g_bitwise_not                      = "~";
char* const g_comma                            = ",";
char* const g_ampersand                        = "&";
char* const g_percent                          = "%";
char* const g_hash                             = "#";
char* const g_empty                            = "";
char* const g_colon                            = ":";
char* const g_question_mark                    = "?";
char* const g_dollar                           = "$";
char* const g_at                               = "@";
char* const g_void                             = "void";
char* const g_i1                               = "i1";
char* const g_i8                               = "i8";
char* const g_i16                              = "i16";
char* const g_i32                              = "i32";
char* const g_i64                              = "i64";
char* const g_u1                               = "u1";
char* const g_u8                               = "u8";
char* const g_u16                              = "u16";
char* const g_u32                              = "u32";
char* const g_u64                              = "u64";
char* const g_double                           = "double";
char* const g_float                            = "float";
char* const g_ret                              = "ret";
char* const g_ptr                              = "ptr";
char* const g_if                               = "if";
char* const g_else                             = "else";
char* const g_while                            = "while";
char* const g_break                            = "break";
char* const g_continue                         = "continue";
char* const g_type                             = "type";
char* const g_defer                            = "defer";
char* const g_new                              = "new";
char* const g_del                              = "del";
char* const g_sizeof                           = "sizeof";
char* const g_comment                          = "//";
char* const g_include                          = "include";
char* const g_opcode_type_def                  = "%%%s = type";
char* const g_opcode_label                     = "L%d:";
char* const g_opcode_branch_if_else            = "br i1 %%%d, label %%L%d, label %%L%d";
char* const g_opcode_branch                    = "br label %%L%d";
char* const g_opcode_target                    = "target triple = \"x86_64-pc-linux-gnu\"";
char* const g_opcode_signed_not                = "%%%d = xor %s %%%d, true";
char* const g_opcode_signed_negative           = "%%%d = mul %s %%%d, -1";
char* const g_opcode_signed_mul                = "%%%d = mul %s %%%d, %%%d";
char* const g_opcode_signed_divide             = "%%%d = sdiv %s %%%d, %%%d";
char* const g_opcode_signed_remainder          = "%%%d = srem %s %%%d, %%%d";
char* const g_opcode_signed_add                = "%%%d = add %s %%%d, %%%d";
char* const g_opcode_signed_sub                = "%%%d = sub %s %%%d, %%%d";
char* const g_opcode_signed_equal_to           = "%%%d = icmp eq %s %%%d, %%%d";
char* const g_opcode_signed_not_equal_to       = "%%%d = icmp ne %s %%%d, %%%d";
char* const g_opcode_signed_less               = "%%%d = icmp slt %s %%%d, %%%d";
char* const g_opcode_signed_less_equal_to      = "%%%d = icmp sle %s %%%d, %%%d";
char* const g_opcode_signed_greater            = "%%%d = icmp sgt %s %%%d, %%%d";
char* const g_opcode_signed_greater_equal_to   = "%%%d = icmp sge %s %%%d, %%%d";
char* const g_opcode_signed_bitwise_or         = "%%%d = or %s %%%d, %%%d";
char* const g_opcode_signed_bitwise_not        = "%%%d = xor %s %%%d, -1";
char* const g_opcode_signed_bitwise_xor        = "%%%d = xor %s %%%d, %%%d";
char* const g_opcode_signed_bitwise_and        = "%%%d = and %s %%%d, %%%d";
char* const g_opcode_signed_shift_left         = "%%%d = shl %s %%%d, %%%d";
char* const g_opcode_signed_shift_rite         = "%%%d = ashr %s %%%d, %%%d";
char* const g_opcode_unsigned_not              = "%%%d = xor %s %%%d, true";
char* const g_opcode_unsigned_negative         = "%%%d = mul %s %%%d, -1";
char* const g_opcode_unsigned_mul              = "%%%d = mul %s %%%d, %%%d";
char* const g_opcode_unsigned_divide           = "%%%d = udiv %s %%%d, %%%d";
char* const g_opcode_unsigned_remainder        = "%%%d = urem %s %%%d, %%%d";
char* const g_opcode_unsigned_add              = "%%%d = add %s %%%d, %%%d";
char* const g_opcode_unsigned_sub              = "%%%d = sub %s %%%d, %%%d";
char* const g_opcode_unsigned_equal_to         = "%%%d = icmp eq %s %%%d, %%%d";
char* const g_opcode_unsigned_not_equal_to     = "%%%d = icmp ne %s %%%d, %%%d";
char* const g_opcode_unsigned_less             = "%%%d = icmp ult %s %%%d, %%%d";
char* const g_opcode_unsigned_less_equal_to    = "%%%d = icmp ule %s %%%d, %%%d";
char* const g_opcode_unsigned_greater          = "%%%d = icmp ugt %s %%%d, %%%d";
char* const g_opcode_unsigned_greater_equal_to = "%%%d = icmp uge %s %%%d, %%%d";
char* const g_opcode_unsigned_bitwise_or       = "%%%d = or %s %%%d, %%%d";
char* const g_opcode_unsigned_bitwise_not      = "%%%d = xor %s %%%d, -1";
char* const g_opcode_unsigned_bitwise_xor      = "%%%d = xor %s %%%d, %%%d";
char* const g_opcode_unsigned_bitwise_and      = "%%%d = and %s %%%d, %%%d";
char* const g_opcode_unsigned_shift_left       = "%%%d = shl %s %%%d, %%%d";
char* const g_opcode_unsigned_shift_rite       = "%%%d = lshr %s %%%d, %%%d";
char* const g_opcode_alloca                    = "%%%d = alloca %s";
char* const g_opcode_flat_gep                  = "%%%d = getelementptr ptr, ptr %%%d, %s %d";
char* const g_opcode_gep                       = "%%%d = getelementptr %s, ptr %%%d, %s %%%d";
char* const g_opcode_sizeof                    = "%%%d = getelementptr %s, ptr null, i64 1";
char* const g_opcode_type_field                = "%%%d = getelementptr inbounds %s, ptr %%%d, i32 0, i32 %d";
char* const g_opcode_ptr_to_int                = "%%%d = ptrtoint ptr %%%d to %s";
char* const g_opcode_int_to_ptr                = "%%%d = inttoptr %s %%%d to ptr";
char* const g_opcode_load_double               = "%%%d = fadd %s %s, 0.0";
char* const g_opcode_load_signed               = "%%%d = add %s %s, 0";
char* const g_opcode_load_character            = "%%%d = add %s %d, 0";
char* const g_opcode_ret                       = "ret %s %%%d";
char* const g_opcode_ret_void                  = "ret %s";
char* const g_opcode_define                    = "define %s @%s";
char* const g_opcode_declare                   = "declare %s @%s";
char* const g_opcode_load                      = "%%%d = load %s, ptr %%%d";
char* const g_opcode_store_direct              = "store %s %s, ptr %%%d ; MEMBER INIT";
char* const g_opcode_store                     = "store %s %%%d, ptr %%%d";
char* const g_opcode_zero_init                 = "store %s zeroinitializer, ptr %%%d";
char* const g_opcode_type_slot                 = "%s %%%d";
char* const g_opcode_call                      = "%%%d = call %s @%s";
char* const g_opcode_void_call                 = "call %s @%s";
char* const g_opcode_entry                     = "entry:";
char* const g_opcode_zero_extend               = "%%%d = zext %s %%%d to %s";
char* const g_opcode_signed_extend             = "%%%d = sext %s %%%d to %s";
char* const g_opcode_trunc                     = "%%%d = trunc %s %%%d to %s";
char* const g_opcode_malloc                    = "%%%d = call ptr @malloc(i64 %%%d)";
char* const g_opcode_free                      = "call void @free(ptr %%%d)";
char* const g_opcode_alloca_string             = "%%%d = alloca [%d x i8]";
char* const g_opcode_store_string              = "store [%d x i8] c\"%s\", ptr %%%d";
char* const g_opcode_increment                 = "%%%d = add %s %%%d, 1";
char* const g_opcode_decrement                 = "%%%d = sub %s %%%d, 1";
char* const g_opcode_floating_negative         = "%%%d = fmul %s %%%d, -1.0";
char* const g_opcode_floating_increment        = "%%%d = fadd %s %%%d, 1.0";
char* const g_opcode_floating_decrement        = "%%%d = fsub %s %%%d, 1.0";
char* const g_opcode_floating_mul              = "%%%d = fmul %s %%%d, %%%d";
char* const g_opcode_floating_div              = "%%%d = fdiv %s %%%d, %%%d";
char* const g_opcode_floating_add              = "%%%d = fadd %s %%%d, %%%d";
char* const g_opcode_floating_sub              = "%%%d = fsub %s %%%d, %%%d";
char* const g_opcode_floating_equal_to         = "%%%d = fcmp oeq %s %%%d, %%%d";
char* const g_opcode_floating_not_equal_to     = "%%%d = fcmp one %s %%%d, %%%d";
char* const g_opcode_floating_less             = "%%%d = fcmp olt %s %%%d, %%%d";
char* const g_opcode_floating_less_equal_to    = "%%%d = fcmp ole %s %%%d, %%%d";
char* const g_opcode_floating_greater          = "%%%d = fcmp ogt %s %%%d, %%%d";
char* const g_opcode_floating_greater_equal_to = "%%%d = fcmp oge %s %%%d, %%%d";
char* const g_opcode_float_to_signed           = "%%%d = fptosi %s %%%d to %s";
char* const g_opcode_float_to_unsigned         = "%%%d = fptoui %s %%%d to %s";
char* const g_opcode_signed_to_floating        = "%%%d = sitofp %s %%%d to %s";
char* const g_opcode_unsigned_to_floating      = "%%%d = uitofp %s %%%d to %s";
char* const g_opcode_floating_trunc            = "%%%d = fptrunc %s %%%d to %s";
char* const g_opcode_floating_extend           = "%%%d = fpext %s %%%d to %s";
char* const g_opcode_signed_to_boolean         = "%%%d = trunc %s %%%d to i1";
char* const g_opcode_unsigned_to_boolean       = "%%%d = trunc %s %%%d to i1";
char* const g_opcode_boolean_to_signed         = "%%%d = zext i1 %%%d to %s";
char* const g_opcode_boolean_to_unsigned       = "%%%d = zext i1 %%%d to %s";

char* g_operator_chars[] = {
    g_dot,
    g_not,
    g_add,
    g_subtract,
    g_divide,
    g_mod,
    g_multiply,
    g_equals,
    g_equal_to,
    g_not_equal_to,
    g_less_equal_to,
    g_less,
    g_greater_equal_to,
    g_greater,
    g_shift_rite,
    g_shift_left,
    g_bitwise_or,
    g_bitwise_xor,
    g_bitwise_and,
    g_bitwise_not,
    g_comma,
    g_ampersand,
    nullptr
};

char* g_builtin_type_keywords[] = {
    g_void,
    g_i1,
    g_i8,
    g_i16,
    g_i32,
    g_i64,
    g_u1,
    g_u8,
    g_u16,
    g_u32,
    g_u64,
    g_double,
    g_float,
    g_ptr,
    nullptr
};

char* g_signed[] = {
    g_i8,
    g_i16,
    g_i32,
    g_i64,
    nullptr
};

char* g_unsigned[] = {
    g_u8,
    g_u16,
    g_u32,
    g_u64,
    nullptr
};

char* g_floating[] = {
    g_double,
    g_float,
    nullptr
};

char* g_boolean[] = {
    g_i1,
    g_u1,
    nullptr
};

char* g_control_keywords[] = {
    g_ret,
    g_if,
    g_else,
    g_while,
    g_defer,
    g_break,
    g_continue,
    nullptr
};

char* g_construct_keywords[] = {
    g_new,
    g_del,
    g_sizeof,
    g_type,
    nullptr
};

char* g_operators_by_precedence[g_precedence_count][g_operator_precedences] = {
    [ g_precedence_arithmetic_0 ]  = { g_multiply, g_divide, g_mod                            },
    [ g_precedence_arithmetic_1 ]  = { g_add, g_subtract                                      },
    [ g_precedence_shift        ]  = { g_shift_left, g_shift_rite                             },
    [ g_precedence_relational_0 ]  = { g_less, g_less_equal_to, g_greater, g_greater_equal_to },
    [ g_precedence_relational_1 ]  = { g_equal_to, g_not_equal_to                             },
    [ g_precedence_bitwise_and  ]  = { g_bitwise_and                                          },
    [ g_precedence_bitwise_xor  ]  = { g_bitwise_xor                                          },
    [ g_precedence_bitwise_or   ]  = { g_bitwise_or                                           },
    [ g_precedence_assignment   ]  = { g_equals                                               },
};

#define len(x) (sizeof(x) / sizeof(*x))

#define list_full(list) ((list)->size == len((list)->begin) - 1)

#define list_append(list, value) \
    if(list_full(list))          \
        quit("list overflow");   \
    else                         \
        (list)->begin[(list)->size++] = value

code_t* get_code()
{
    return &g_file.code[g_file.module];
}

void code_rewind(int by)
{
    get_code()->at -= by;
}

void quit(char* format, ...)
{
    auto out = stderr;
    auto args = (va_list) {};
    va_start(args, format);
    fprintf(out, "%s%s: line %d:%s %serror: %s", g_white, get_code()->path.begin, get_code()->line, g_normal, g_red, g_normal);
    vfprintf(out, format, args);
    fprintf(out, g_escape_newline);
    va_end(args);
    exit(1);
}

void emit(char* format, ...)
{
    auto out = stdout;
    auto args = (va_list) {};
    va_start(args, format);
    for(auto tab = 0; tab < g_file.tabs; tab++)
    {
        fprintf(out, g_escape_tab);
    }
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, g_escape_newline);
}

bool is_digit_char(char c)
{
    return c >= *g_digit_begin
        && c <= *g_digit_end;
}

bool is_numeric_char(char c)
{
    return is_digit_char(c) || c == *g_dot;
}

bool is_not_semicolon(char c)
{
    return c != *g_semicolon;
}

bool is_lower_char(char c)
{
    return c >= *g_lower_begin
        && c <= *g_lower_end;
}

bool is_upper_char(char c)
{
    return c >= *g_upper_begin
        && c <= *g_upper_end;
}

bool is_alpha_char(char c)
{
    return is_lower_char(c)
        || is_upper_char(c);
}

bool is_alnum_char(char c)
{
    return is_alpha_char(c)
        || is_digit_char(c)
        || c == *g_underscore;
}

bool is_escape_char(char c)
{
    return c == *g_escape_alert
        || c == *g_escape_backspace
        || c == *g_escape_form_feed
        || c == *g_escape_newline
        || c == *g_escape_carriage_return
        || c == *g_escape_tab
        || c == *g_escape_vertical_tab
        || c == *g_escape_backslash
        || c == *g_escape_apostrophe
        || c == *g_escape_question_mark
        /* quotation mark (") is removed to simplify read_chars() */
        || c == *g_escape_null;
}

bool is_space_char(char c)
{
    return c == *g_space
        || c == *g_escape_newline
        || c == *g_escape_tab;
}

bool is_string_char(char c)
{
    return is_alnum_char(c)
        || is_escape_char(c)
        || is_space_char(c)
        || c == *g_dot
        || c == *g_divide
        || c == *g_mod
        || c == *g_subtract
        || c == *g_add
        || c == *g_equals
        || c == *g_colon
        || c == *g_semicolon
        || c == *g_comma
        || c == *g_question_mark
        || c == *g_not
        || c == *g_bitwise_or
        || c == *g_bitwise_and
        || c == *g_bitwise_xor
        || c == *g_bitwise_not
        || c == *g_hash
        || c == *g_dollar
        || c == *g_at
        || c == *g_left_paren
        || c == *g_rite_paren
        || c == *g_left_square
        || c == *g_rite_square
        || c == *g_left_curl
        || c == *g_rite_curl;
}

bool is_operator_char(char c)
{
    for(auto operator = g_operator_chars; *operator; operator++)
    {
        if(strchr(*operator, c))
        {
            return true;
        }
    }
    return false;
}

bool str_equal(char* str, char* other)
{
    return strcmp(str, other) == 0;
}

bool str_in(str_t str, char** array)
{
    while(*array)
    {
        if(str_equal(str.begin, *array))
        {
            return true;
        }
        array += 1;
    }
    return false;
}

char** get_operators(precedence_t precedence)
{
    if(precedence >= g_precedence_count)
    {
        quit("unknown precedence level");
    }
    return g_operators_by_precedence[precedence];
}

bool is_relational(str_t operator)
{
    return str_in(operator, get_operators(g_precedence_relational_0))
        || str_in(operator, get_operators(g_precedence_relational_1));
}

bool is_assignment_operator(str_t operator)
{
    return str_in(operator, get_operators(g_precedence_assignment));
}

bool is_builtin_type_name(str_t type_name)
{
    return str_in(type_name, g_builtin_type_keywords);
}

bool is_aggregate_type_name(str_t type_name)
{
    for(auto i = 0; i < g_file.types.size; i++)
    {
        if(str_equal(type_name.begin, g_file.types.begin[i].name.begin))
        {
            return true;
        }
    }
    return false;
}

bool is_type_name(str_t type_name)
{
    return is_builtin_type_name(type_name) || is_aggregate_type_name(type_name);
}

bool is_regular_pointer(type_t type)
{
    return type.stars > 0;
}

bool is_generic_pointer(type_t type)
{
    return str_equal(type.name.begin, g_ptr);
}

bool is_pointer(type_t type)
{
    return is_regular_pointer(type) || is_generic_pointer(type);
}

bool is_aggregate(type_t type)
{
    return !is_pointer(type) && is_aggregate_type_name(type.name);
}

bool is_boolean(type_t type)
{
    return !is_pointer(type) && str_in(type.name, g_boolean);
}

bool is_floating(type_t type)
{
    return !is_pointer(type) && str_in(type.name, g_floating);
}

bool is_size(type_t type)
{
    return !is_pointer(type) && (str_equal(type.name.begin, g_i64) || str_equal(type.name.begin, g_u64));
}

bool is_signed(type_t type)
{
    return !is_pointer(type) && str_in(type.name, g_signed);
}

bool is_unsigned(type_t type)
{
    return !is_pointer(type) && str_in(type.name, g_unsigned);
}

bool is_integral(type_t type)
{
    return is_signed(type) || is_unsigned(type);
}

bool is_numeric(type_t type)
{
    return is_integral(type) || is_floating(type);
}

bool is_member_init(type_t type)
{
    return is_integral(type) || is_floating(type) || is_boolean(type);
}

void str_append(str_t* str, char* chars)
{
    while(*chars)
    {
        list_append(str, *chars);
        chars += 1;
    }
}

str_t str_init(char* chars)
{
    auto str = (str_t) {};
    str_append(&str, chars);
    return str;
}

char* get_builtin_prefix(type_t type)
{
    return str_in(type.name, g_builtin_type_keywords) ? g_empty : g_percent;
}

str_t to_llvm_type(type_t type)
{
    if(is_pointer(type))
    {
        return str_init(g_ptr);
    }
    else
    {
        auto llvm_type = type;
        if(is_unsigned(llvm_type) || is_boolean(llvm_type))
        {
            llvm_type.name.begin[0] = 'i';
        }
        auto prefix = get_builtin_prefix(llvm_type);
        auto out = str_init(prefix);
        str_append(&out, llvm_type.name.begin);
        return out;
    }
}

value_t* value_in_list(str_t str, value_list_t* list)
{
    for(auto i = 0; i < list->size; i++)
    {
        auto check = &list->begin[i];
        if(str_equal(str.begin, check->name.begin))
        {
            return check;
        }
    }
    return nullptr;
}

str_t* str_in_list(str_t str, str_list_t* list)
{
    for(auto i = 0; i < list->size; i++)
    {
        auto check = &list->begin[i];
        if(str_equal(str.begin, check->begin))
        {
            return check;
        }
    }
    return nullptr;
}

bool is_reserved_keyword(str_t keyword)
{
    return is_type_name(keyword)
        || str_in(keyword, g_control_keywords)
        || str_in(keyword, g_construct_keywords);
}

int get_block()
{
    g_file.block += 1;
    return g_file.block;
}

int get_slot()
{
    g_file.slot += 1;
    return g_file.slot;
}

int get_label()
{
    g_file.label += 1;
    return g_file.label;
}

char peek_char()
{
    auto at = get_code()->at;
    return get_code()->begin[at];
}

int code_last_index()
{
    return g_code_size - 1;
}

void step()
{
    get_code()->at += 1;
    if(get_code()->at == code_last_index())
    {
        quit("unexpected end of file");
    }
}

str_t to_stars(type_t value)
{
    auto stars = (str_t) {};
    while(value.stars-- > 0)
    {
        list_append(&stars, '*');
    }
    return stars;
}

str_t to_print_type(type_t type)
{
    auto print = (str_t) {};
    str_append(&print, type.name.begin);
    str_append(&print, to_stars(type).begin);
    return print;
}

void assert_types_match(type_t left, type_t rite, str_t operator)
{
    auto print_type_a = to_print_type(left).begin;
    auto print_type_b = to_print_type(rite).begin;
    auto stars_match = left.stars == rite.stars;
    auto type_names_match = str_equal(left.name.begin, rite.name.begin);
    if(!stars_match || !type_names_match)
    {
        quit("types '%s' and '%s' mismatch with operator '%s'", print_type_a, print_type_b, operator.begin);
    }
}

void assert_type(type_t type, str_t operator, bool with(type_t))
{
    if(!with(type))
    {
        auto print_type = to_print_type(type).begin;
        quit("'%s' does not support operator '%s'", print_type, operator.begin);
    }
}

bool is_rvalue(value_t value)
{
    return !value.is_lvalue;
}

void assert_lvalue(value_t value, str_t operator)
{
    if(is_rvalue(value))
    {
        auto print_type = to_print_type(value.type).begin;
        quit("expected lvalue with '%s' and operator '%s'", print_type, operator.begin);
    }
}

int skip_space_and_comment()
{
    auto len = (int) strlen(g_comment);
    auto count = 0;
    while(true)
    {
        auto c = peek_char();
        if(is_space_char(c))
        {
            if(c == *g_escape_newline)
            {
                get_code()->line += 1;
            }
            step();
            count += 1;
        }
        else
        if(c == *g_divide)
        {
            step();
            if(peek_char() != *g_divide)
            {
                code_rewind(1);
                count -= len;
                break;
            }
            step();
            count += len;
            while(peek_char() != *g_escape_newline)
            {
                step();
                count += 1;
            }
        }
        else
        {
            break;
        }
    }
    return count;
}

char next_char()
{
    skip_space_and_comment();
    return peek_char();
}

bool is_end_of_file()
{
    skip_space_and_comment();
    return get_code()->at == get_code()->size;
}

void match(char* expected)
{
    auto size = (int) strlen(expected);
    str_t got = {};
    for(auto i = 0; i < size; i++)
    {
        list_append(&got, next_char());
        step();
    }
    if(!str_equal(got.begin, expected))
    {
        quit("expected '%s' but got '%s'", expected, got.begin);
    }
}

char convert_escape_char(char ch)
{
    if(ch ==  'a') return *g_escape_alert;
    if(ch ==  'b') return *g_escape_backspace;
    if(ch ==  'f') return *g_escape_form_feed;
    if(ch ==  'n') return *g_escape_newline;
    if(ch ==  'r') return *g_escape_carriage_return;
    if(ch ==  't') return *g_escape_tab;
    if(ch ==  'v') return *g_escape_vertical_tab;
    if(ch == '\\') return *g_escape_backslash;
    if(ch == '\'') return *g_escape_apostrophe;
    if(ch == '\"') return *g_escape_quotation;
    if(ch ==  '?') return *g_escape_question_mark;
    if(ch ==  '0') return *g_escape_null;
    quit("'%c' is an invalid escape character", ch);
    return 0;
}

str_t read_chars(bool matches(char))
{
    auto str = (str_t) {};
    while(true)
    {
        auto c = peek_char();
        if(c == *g_escape_backslash)
        {
            step();
            c = convert_escape_char(peek_char());
        }
        if(matches(c))
        {
            step();
            list_append(&str, c);
        }
        else
        {
            break;
        }
    }
    return str;
}

str_t read_operator()
{
    skip_space_and_comment();
    return read_chars(is_operator_char);
}

str_t read_alnum()
{
    skip_space_and_comment();
    return read_chars(is_alnum_char);
}

str_t read_string()
{
    skip_space_and_comment();
    match(g_escape_quotation);
    auto out = read_chars(is_string_char);
    match(g_escape_quotation);
    return out;
}

str_t read_numeric()
{
    skip_space_and_comment();
    return read_chars(is_numeric_char);
}

str_t read_till_semicolon()
{
    return read_chars(is_not_semicolon);
}

str_t peek_operator()
{
    auto operator = read_operator();
    code_rewind(operator.size);
    return operator;
}

str_t peek_alnum()
{
    auto alnum = read_alnum();
    code_rewind(alnum.size);
    return alnum;
}

int read_stars()
{
    auto stars = 0;
    while(true)
    {
        auto c = next_char();
        if(c == *g_left_square)
        {
            step();
            auto d = next_char();
            if(d == *g_rite_square)
            {
                stars += 1;
                step();
            }
            else
            {
                code_rewind(1);
                break;
            }
        }
        else
        if(c == *g_multiply)
        {
            stars += 1;
            step();
        }
        else
        {
            break;
        }
    }
    return stars;
}

type_t read_type()
{
    auto type = (type_t) {
        .name = read_alnum(),
        .stars = read_stars(),
    };
    return type;
}

value_t read_value()
{
    auto value = (value_t) {
        .type = read_type(),
        .name = read_alnum(),
        .slot = get_slot(),
    };
    return value;
}

value_t read_value_decl()
{
    auto value = read_value();
    if(value_in_list(value.name, &g_file.values))
    {
        quit("'%s' already declared", value.name.begin);
    }
    if(is_reserved_keyword(value.name))
    {
        quit("'%s' is a reserved keyword", value.name.begin);
    }
    if(!is_type_name(value.type.name))
    {
        quit("'%s' not a valid type", value.type.name.begin);
    }
    return value;
}

value_list_t read_function_decl_arg_list()
{
    auto values = (value_list_t) {};
    match(g_left_paren);
    while(true)
    {
        if(next_char() == *g_rite_paren)
        {
            break;
        }
        auto arg = read_value_decl();
        list_append(&values, arg);
        if(next_char() == *g_comma)
        {
            match(g_comma);
            if(next_char() == *g_rite_paren)
            {
                quit("expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(g_rite_paren);
    return values;
}

value_t read_expression();

void execute_defers(int total)
{
    auto at = get_code()->at;
    for(auto i = 0; i < total; i++)
    {
        get_code()->at = g_file.defers.begin[g_file.defers.size - 1 - i].at;
        read_expression();
    }
    get_code()->at = at;
}

value_t read_ret_statement(value_t ret_value)
{
    read_alnum();
    if(str_equal(ret_value.type.name.begin, g_void))
    {
        match(g_semicolon);
        auto value = (value_t) {
            .type.name = str_init(g_void)
        };
        execute_defers(g_file.defers.size);
        emit(g_opcode_ret_void, g_void);
        return value;
    }
    else
    {
        auto value = read_expression();
        match(g_semicolon);
        auto llvm_type = to_llvm_type(value.type).begin;
        execute_defers(g_file.defers.size);
        emit(g_opcode_ret, llvm_type, value.slot);
        return value;
    }
}

bool read_statement(value_t, scope_t, int);

void read_if_statement(value_t ret_value, branch_t branch)
{
    read_alnum();
    match(g_left_paren);
    auto value = read_expression();
    auto operator = str_init(g_if);
    assert_type(value.type, operator, is_boolean);
    match(g_rite_paren);
    emit(g_opcode_branch_if_else, value.slot, branch.if_label, branch.else_label);
    emit(g_opcode_label, branch.if_label);
    auto terminated = read_statement(ret_value, g_scope_if, branch.block);
    if(!terminated)
    {
        emit(g_opcode_branch, branch.end_label);
    }
}

void read_else_statement(value_t ret_value, branch_t branch)
{
    emit(g_opcode_label, branch.else_label);
    auto keyword = peek_alnum();
    if(str_equal(keyword.begin, g_else))
    {
        read_alnum();
        auto terminated = read_statement(ret_value, g_scope_if, branch.block);
        if(!terminated)
        {
            emit(g_opcode_branch, branch.end_label);
        }
    }
    else
    {
        emit(g_opcode_branch, branch.end_label);
    }
}

void read_if_else_statement(value_t ret_value, int block)
{
    auto branch = (branch_t) {
        .if_label = get_label(),
        .else_label = get_label(),
        .end_label = get_label(),
        .block = block,
    };
    read_if_statement(ret_value, branch);
    read_else_statement(ret_value, branch);
    emit(g_opcode_label, branch.end_label);
}

void read_defer_statement(scope_t scope, int block)
{
    read_alnum();
    auto defer = (defer_t) {
        .scope = scope,
        .at = get_code()->at,
        .block = block
    };
    list_append(&g_file.defers, defer);
    read_till_semicolon();
    match(g_semicolon);
}

void execute_break_defers()
{
    auto scopes = g_file.defers.begin;
    auto defers = 0;
    auto at = g_file.defers.size - 1;
    if(scopes[at].scope != g_scope_function)
    {
        while(at >= 0)
        {
            if(scopes[at].scope == g_scope_while)
            {
                break;
            }
            at -= 1;
            defers += 1;
        }
        auto block = scopes[at].block;
        while(at >= 0)
        {
            if(scopes[at].block != block)
            {
                break;
            }
            at -= 1;
            defers += 1;
        }
        execute_defers(defers);
    }
}

void read_break_statement()
{
    read_alnum();
    match(g_semicolon);
    if(g_file.loop_end.size == 0)
    {
        quit("%s statement not within a loop", g_break);
    }
    execute_break_defers();
    auto to = g_file.loop_end.begin[g_file.loop_end.size - 1];
    emit(g_opcode_branch, to);
}

void read_continue_statement()
{
    read_alnum();
    match(g_semicolon);
    if(g_file.loop_again.size == 0)
    {
        quit("%s statement not within a loop", g_continue);
    }
    execute_break_defers();
    auto to = g_file.loop_again.begin[g_file.loop_again.size - 1];
    emit(g_opcode_branch, to);
}

void read_while_statement(value_t ret_value, int block)
{
    auto again_label = get_label();
    auto while_label = get_label();
    auto end_label = get_label();
    list_append(&g_file.loop_again, again_label);
    list_append(&g_file.loop_end, end_label);
    emit(g_opcode_branch, again_label);
    emit(g_opcode_label, again_label);
    read_alnum();
    match(g_left_paren);
    auto value = read_expression();
    match(g_rite_paren);
    auto operator = str_init(g_while);
    assert_type(value.type, operator, is_boolean);
    emit(g_opcode_branch_if_else, value.slot, while_label, end_label);
    emit(g_opcode_label, while_label);
    auto terminated = read_statement(ret_value, g_scope_while, block);
    if(!terminated)
    {
        emit(g_opcode_branch, again_label);
    }
    emit(g_opcode_label, end_label);
    g_file.loop_again.size -= 1;
    g_file.loop_end.size -= 1;
}

value_t field_index(value_t, str_t);

void default_init_aggregate(value_t value)
{
    if(is_aggregate(value.type))
    {
        auto members = value_in_list(value.type.name, &g_file.types);
        for(auto i = 0; i < members->init.size; i++)
        {
            auto name = members->names.begin[i];
            auto type = members->types.begin[i];
            auto init = members->init.begin[i];
            auto field = field_index(value, name);
            if(is_member_init(field.type))
            {
                if(!str_equal(init.begin, g_null))
                {
                    auto llvm_type = to_llvm_type(type).begin;
                    emit(g_opcode_store_direct, llvm_type, init.begin, field.slot);
                }
            }
            default_init_aggregate(field);
        }
    }
}

value_t alloca_value(value_t value)
{
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_alloca, value.slot, llvm_type);
    emit(g_opcode_zero_init, llvm_type, value.slot);
    default_init_aggregate(value);
    return value;
}

value_t declare_local_value()
{
    auto value = read_value_decl();
    list_append(&g_file.values, value);
    return alloca_value(value);
}

bool read_block(value_t, scope_t);

bool read_statement(value_t ret_value, scope_t scope, int block)
{
    auto keyword = peek_alnum();
    if(str_in(keyword, g_control_keywords))
    {
        if(str_equal(keyword.begin, g_ret))
        {
            auto operator = str_init(g_ret);
            auto value = read_ret_statement(ret_value);
            assert_types_match(value.type, ret_value.type, operator);
            return true;
        }
        if(str_equal(keyword.begin, g_continue))
        {
            read_continue_statement();
            return true;
        }
        if(str_equal(keyword.begin, g_break))
        {
            read_break_statement();
            return true;
        }
        if(str_equal(keyword.begin, g_defer))
        {
            read_defer_statement(scope, block);
        }
        if(str_equal(keyword.begin, g_if))
        {
            read_if_else_statement(ret_value, block);
        }
        if(str_equal(keyword.begin, g_else))
        {
            quit("missing binding '%s'", g_if);
        }
        if(str_equal(keyword.begin, g_while))
        {
            read_while_statement(ret_value, block);
        }
    }
    else
    if(is_type_name(keyword))
    {
        auto value = declare_local_value();
        auto read = skip_space_and_comment();
        if(next_char() == *g_semicolon)
        {
            match(g_semicolon);
        }
        else
        {
            code_rewind(read);
            code_rewind(value.name.size);
            read_expression();
            match(g_semicolon);
        }
    }
    else
    {
        if(next_char() == *g_left_curl)
        {
            return read_block(ret_value, scope);
        }
        else
        {
            read_expression();
            match(g_semicolon);
        }
    }
    return false;
}

bool read_block(value_t ret_value, scope_t scope)
{
    auto block = get_block();
    auto defers = g_file.defers.size;
    auto values = g_file.values.size;
    bool terminated = false;
    g_file.tabs += 1;
    match(g_left_curl);
    while(true)
    {
        if(next_char() == *g_rite_curl)
        {
            break;
        }
        if(terminated)
        {
            quit("block was terminated");
        }
        terminated = read_statement(ret_value, scope, block);
    }
    if(!terminated)
    {
        execute_defers(g_file.defers.size - defers);
    }
    match(g_rite_curl);
    g_file.tabs -= 1;
    g_file.defers.size = defers;
    g_file.values.size = values;
    return terminated;
}

void emit_parameters(value_list_t* args, value_t* ret_value)
{
    emit(g_str, g_left_paren);
    for(auto i = 0; i < args->size; i++)
    {
        auto type = args->begin[i].type;
        auto slot = args->begin[i].slot;
        list_append(&ret_value->types, type);
        auto llvm_type = to_llvm_type(type).begin;
        emit(g_opcode_type_slot, llvm_type, slot);
        if(i < args->size - 1)
        {
            emit(g_str, g_comma);
        }
    }
    emit(g_str, g_rite_paren);
}

void read_function()
{
    g_file.slot = 0;
    auto ret_value = read_value_decl();
    auto llvm_type = to_llvm_type(ret_value.type).begin;
    if(next_char() == *g_not)
    {
        match(g_not);
        ret_value.unlock_args = true;
    }
    auto args = read_function_decl_arg_list();
    if(next_char() == *g_semicolon)
    {
        emit(g_opcode_declare, llvm_type, ret_value.name.begin);
        emit_parameters(&args, &ret_value);
        list_append(&g_file.values, ret_value);
        match(g_semicolon);
    }
    else
    {
        emit(g_opcode_define, llvm_type, ret_value.name.begin);
        emit_parameters(&args, &ret_value);
        list_append(&g_file.values, ret_value);
        emit(g_str, g_left_curl);
        emit(g_str, g_opcode_entry);
        for(auto i = 0; i < args.size; i++)
        {
            auto arg = args.begin[i];
            auto slot = get_slot();
            auto llvm_type = to_llvm_type(arg.type).begin;
            emit(g_opcode_alloca, slot, llvm_type);
            emit(g_opcode_store, llvm_type, arg.slot, slot);
            arg.slot = slot;
            list_append(&g_file.values, arg);
        }
        bool terminated = read_block(ret_value, g_scope_function);
        if(!terminated)
        {
            quit("block missing '%s' statement", g_ret);
        }
        emit(g_str, g_rite_curl);
        g_file.values.size -= args.size;
    }
    emit(g_str, g_empty);
}

void read_type_def()
{
    read_alnum();
    auto type_name = read_alnum();
    auto value = (value_t) {
        .type = {
            .name = type_name,
        },
        .name = type_name,
    };
    if(value_in_list(value.name, &g_file.types))
    {
        quit("type '%s' already declared", value.name.begin);
    }
    emit(g_opcode_type_def, type_name.begin);
    emit(g_str, g_left_curl);
    match(g_left_curl);
    while(true)
    {
        if(next_char() == *g_rite_curl)
        {
            break;
        }
        auto type = read_type();
        auto name = read_alnum();
        if(!is_type_name(type.name))
        {
            quit("unknown type '%s' in '%s'", type.name.begin, type_name.begin);
        }
        if(str_in_list(name, &value.names))
        {
            quit("duplicate member '%s' in '%s'", name.begin, type_name.begin);
        }
        if(next_char() == *g_equals)
        {
            auto operator = str_init(g_equals);
            assert_type(type, operator, is_member_init);
            match(g_equals);
            auto numeric = read_numeric();
            list_append(&value.init, numeric);
        }
        else
        {
            auto null = str_init(g_null);
            list_append(&value.init, null);
        }
        match(g_semicolon);
        list_append(&value.types, type);
        list_append(&value.names, name);
        auto llvm_type = to_llvm_type(type).begin;
        emit(g_str, llvm_type);
        if(next_char() != *g_rite_curl)
        {
            emit(g_str, g_comma);
        }
    }
    emit(g_str, g_rite_curl);
    match(g_rite_curl);
    match(g_semicolon);
    list_append(&g_file.types, value);
}

value_t to_rvalue(value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = get_slot();
        auto llvm_type = to_llvm_type(value.type).begin;
        emit(g_opcode_load, slot, llvm_type, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

value_t load_indirect(value_t found)
{
    auto value = (value_t) {
        .is_lvalue = true,
        .slot = get_slot(),
        .type = found.type,
    };
    emit(g_opcode_flat_gep, value.slot, found.slot, g_i64, 0);
    return value;
}

value_t load_direct()
{
    auto value = (value_t) {
        .slot = get_slot(),
    };
    auto name = read_numeric();
    if(strchr(name.begin, *g_dot))
    {
        value.type.name = str_init(g_double);
        emit(g_opcode_load_double, g_file.slot, value.type.name.begin, name.begin);
    }
    else
    {
        value.type.name = str_init(g_i64);
        emit(g_opcode_load_signed, g_file.slot, value.type.name.begin, name.begin);
    }
    return value;
}

str_t fix_escape_chars(str_t string, int* size)
{
    auto out = (str_t) {};
    for(auto i = 0; i < string.size + 1; i++)
    {
        auto ch = string.begin[i];
        if(ch == *g_escape_alert)
        {
            str_append(&out, g_llvm_escape_alert);
        }
        else
        if(ch == *g_escape_backspace)
        {
            str_append(&out, g_llvm_escape_backspace);
        }
        else
        if(ch == *g_escape_tab)
        {
            str_append(&out, g_llvm_escape_tab);
        }
        else
        if(ch == *g_escape_newline)
        {
            str_append(&out, g_llvm_escape_newline);
        }
        else
        if(ch == *g_escape_vertical_tab)
        {
            str_append(&out, g_llvm_escape_vertical_tab);
        }
        else
        if(ch == *g_escape_form_feed)
        {
            str_append(&out, g_llvm_escape_form_feed);
        }
        else
        if(ch == *g_escape_carriage_return)
        {
            str_append(&out, g_llvm_escape_carriage_return);
        }
        else
        if(ch == *g_escape_quotation)
        {
            str_append(&out, g_llvm_escape_quotation);
        }
        else
        if(ch == *g_escape_null)
        {
            str_append(&out, g_llvm_escape_null);
        }
        else
        if(ch == *g_escape_backslash)
        {
            str_append(&out, g_llvm_escape_backslash);
        }
        else
        if(ch == *g_escape_question_mark)
        {
            str_append(&out, g_llvm_escape_question_mark);
        }
        else
        {
            list_append(&out, ch);
        }
        *size += 1;
    }
    return out;
}

slot_list_t read_function_call_arg_list(type_list_t* types)
{
    auto list = (slot_list_t) {};
    match(g_left_paren);
    while(true)
    {
        if(next_char() == *g_rite_paren)
        {
            break;
        }
        auto value = read_expression();
        auto slot = g_file.slot;
        list_append(&list, slot);
        list_append(types, value.type);
        if(next_char() == *g_comma)
        {
            match(g_comma);
            if(next_char() == *g_rite_paren)
            {
                quit("expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(g_rite_paren);
    return list;
}

value_t increment(value_t value, bool prefix)
{
    auto slot = value.slot;
    auto operator = str_init(g_increment);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_numeric);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(out.type).begin;
    is_floating(value.type)
        ? emit(g_opcode_floating_increment, out.slot, llvm_type, value.slot)
        : emit(g_opcode_increment, out.slot, llvm_type, value.slot);
    emit(g_opcode_store, llvm_type, out.slot, slot);
    return prefix ? out : value;
}

value_t prefix_increment(value_t value)
{
    return increment(value, true);
}

value_t postfix_increment(value_t value)
{
    return increment(value, false);
}

value_t decrement(value_t value, bool prefix)
{
    auto slot = value.slot;
    auto operator = str_init(g_decrement);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_numeric);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(out.type).begin;
    is_floating(value.type)
        ? emit(g_opcode_floating_decrement, out.slot, llvm_type, value.slot)
        : emit(g_opcode_decrement, out.slot, llvm_type, value.slot);
    emit(g_opcode_store, llvm_type, out.slot, slot);
    return prefix ? out : value;
}

value_t prefix_decrement(value_t value)
{
    return decrement(value, true);
}

value_t postfix_decrement(value_t value)
{
    return decrement(value, false);
}

value_t get_address_of(value_t value)
{
    auto operator = str_init(g_ampersand);
    assert_lvalue(value, operator);
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

value_t dereference(value_t value)
{
    if(is_pointer(value.type))
    {
        if(value.is_lvalue)
        {
            auto llvm_type = to_llvm_type(value.type).begin;
            auto slot = get_slot();
            emit(g_opcode_load, slot, llvm_type, value.slot);
            value.slot = slot;
        }
        value.type.stars -= 1;
        value.is_lvalue = true;
        return value;
    }
    quit("expected pointer type for '%s' while derefencing", value.type.name.begin);
    return (value_t) {};
}

value_t to_positive(value_t value)
{
    auto operator = str_init(g_add);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_numeric);
    return value;
}

value_t to_negative(value_t value)
{
    auto operator = str_init(g_subtract);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_numeric);
    auto out = (value_t) {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    is_floating(out.type)
        ? emit(g_opcode_floating_negative, out.slot, llvm_type, value.slot)
        : emit(g_opcode_signed_negative, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_bitwise_not(value_t value)
{
    auto operator = str_init(g_bitwise_not);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_integral);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_signed_bitwise_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_not(value_t value)
{
    auto operator = str_init(g_not);
    value = to_rvalue(value);
    assert_type(value.type, operator, is_boolean);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_signed_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_sizeof(value_t value)
{
    value = to_rvalue(value);
    auto slot = get_slot();
    auto out = (value_t) {
        .slot = get_slot(),
        .type.name = str_init(g_i64),
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_sizeof, slot, llvm_type);
    emit(g_opcode_ptr_to_int, out.slot, slot, g_i64);
    return out;
}

value_t to_type_sizeof(type_t type)
{
    auto slot = get_slot();
    auto out = (value_t) {
        .slot = get_slot(),
        .type.name = str_init(g_i64),
    };
    auto llvm_type = to_llvm_type(type).begin;
    emit(g_opcode_sizeof, slot, llvm_type);
    emit(g_opcode_ptr_to_int, out.slot, slot, g_i64);
    return out;
}

value_t pointer_up(type_t type)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    out.type.stars += 1;
    return out;
}

value_t to_new(type_t type)
{
    auto size = to_type_sizeof(type);
    if(next_char() == *g_left_square)
    {
        match(g_left_square);
        value_t elems = read_expression();
        match(g_rite_square);
        auto total = (value_t) {
            .slot = get_slot(),
        };
        auto operator = str_init(g_index);
        assert_type(elems.type, operator, is_size);
        emit(g_opcode_signed_mul, total.slot, g_i64, elems.slot, size.slot);
        auto out = pointer_up(type);
        emit(g_opcode_malloc, out.slot, total.slot);
        return out;
    }
    else
    {
        auto out = pointer_up(type);
        emit(g_opcode_malloc, out.slot, size.slot);
        return out;
    }
}

value_t do_del(value_t value)
{
    auto operator = str_init(g_del);
    if(is_pointer(value.type))
    {
        value = to_rvalue(value);
        emit(g_opcode_free, value.slot);
        auto out = (value_t) {
            .type.name = str_init(g_void)
        };
        return out;
    }
    quit("expected pointer with '%s' operator", operator.begin);
    return (value_t) {};
}

typedef enum
{
    g_power_1,
    g_power_8,
    g_power_16,
    g_power_32,
    g_power_64,
    g_power_float,
    g_power_double,
}
type_power_t;

type_power_t type_power(type_t type)
{
    auto at = type.name.begin;
    if(str_equal(at, g_i1)
    || str_equal(at, g_u1))
    {
        return g_power_1;
    }
    if(str_equal(at, g_i8)
    || str_equal(at, g_u8))
    {
        return g_power_8;
    }
    if(str_equal(at, g_i16)
    || str_equal(at, g_u16))
    {
        return g_power_16;
    }
    if(str_equal(at, g_i32)
    || str_equal(at, g_u32))
    {
        return g_power_32;
    }
    if(str_equal(at, g_i64)
    || str_equal(at, g_u64))
    {
        return g_power_64;
    }
    if(str_equal(at, g_float))
    {
        return g_power_float;
    }
    if(str_equal(at, g_double))
    {
        return g_power_double;
    }
    quit("unknown type power '%s'", at);
    return 0;
}

value_t pointer_to_pointer(value_t value, type_t type)
{
    auto out = value;
    out.type = type;
    return out;
}

value_t boolean_to_boolean(value_t value, type_t type)
{
    auto out = value;
    out.type = type;
    return out;
}

value_t signed_to_pointer(value_t value, type_t type)
{
    auto llvm_type = to_llvm_type(value.type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_int_to_ptr, out.slot, llvm_type, value.slot);
    return out;
}

value_t unsigned_to_pointer(value_t value, type_t type)
{
    auto llvm_type = to_llvm_type(value.type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_int_to_ptr, out.slot, llvm_type, value.slot);
    return out;
}

value_t integral_to_integral(value_t value, type_t type, bool from_signed)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    if(type_power(type) > type_power(value.type))
    {
        auto extend = from_signed ? g_opcode_signed_extend : g_opcode_zero_extend;
        auto out = (value_t) {
            .slot = get_slot(),
            .type = type,
        };
        emit(extend, out.slot, llvm_type_a, value.slot, llvm_type_b);
        return out;
    }
    if(type_power(type) < type_power(value.type))
    {
        auto out = (value_t) {
            .slot = get_slot(),
            .type = type,
        };
        emit(g_opcode_trunc, out.slot, llvm_type_a, value.slot, llvm_type_b);
        return out;
    }
    value.type = type;
    return value;
}

value_t signed_to_signed(value_t value, type_t type)
{
    return integral_to_integral(value, type, true);
}

value_t signed_to_unsigned(value_t value, type_t type)
{
    return integral_to_integral(value, type, true);
}

value_t unsigned_to_unsigned(value_t value, type_t type)
{
    return integral_to_integral(value, type, false);
}

value_t unsigned_to_signed(value_t value, type_t type)
{
    return integral_to_integral(value, type, false);
}

value_t floating_to_signed(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_float_to_signed, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t floating_to_unsigned(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_float_to_unsigned, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t signed_to_floating(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_signed_to_floating, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t signed_to_boolean(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_signed_to_boolean, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t unsigned_to_boolean(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_unsigned_to_boolean, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t boolean_to_signed(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_boolean_to_signed, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t boolean_to_unsigned(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_boolean_to_unsigned, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t unsigned_to_floating(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    auto out = (value_t) {
        .slot = get_slot(),
        .type = type,
    };
    emit(g_opcode_unsigned_to_floating, out.slot, llvm_type_a, value.slot, llvm_type_b);
    return out;
}

value_t floating_to_floating(value_t value, type_t type)
{
    auto llvm_type_a = to_llvm_type(value.type).begin;
    auto llvm_type_b = to_llvm_type(type).begin;
    if(type_power(type) < type_power(value.type))
    {
        auto out = (value_t) {
            .slot = get_slot(),
            .type = type,
        };
        emit(g_opcode_floating_trunc, out.slot, llvm_type_a, value.slot, llvm_type_b);
        return out;
    }
    if(type_power(type) > type_power(value.type))
    {
        auto out = (value_t) {
            .slot = get_slot(),
            .type = type,
        };
        emit(g_opcode_floating_extend, out.slot, llvm_type_a, value.slot, llvm_type_b);
        return out;
    }
    return value;
}

value_t type_cast(value_t value, type_t type)
{
    value = to_rvalue(value);
    if(is_pointer(value.type))
    {
        if(is_pointer(type))
        {
            return pointer_to_pointer(value, type);
        }
    }
    if(is_signed(value.type))
    {
        if(is_pointer(type))
        {
            return signed_to_pointer(value, type);
        }
        if(is_signed(type))
        {
            return signed_to_signed(value, type);
        }
        if(is_unsigned(type))
        {
            return signed_to_unsigned(value, type);
        }
        if(is_floating(type))
        {
            return signed_to_floating(value, type);
        }
        if(is_boolean(type))
        {
            return signed_to_boolean(value, type);
        }
    }
    if(is_unsigned(value.type))
    {
        if(is_pointer(type))
        {
            return unsigned_to_pointer(value, type);
        }
        if(is_signed(type))
        {
            return unsigned_to_signed(value, type);
        }
        if(is_unsigned(type))
        {
            return unsigned_to_unsigned(value, type);
        }
        if(is_floating(type))
        {
            return unsigned_to_floating(value, type);
        }
        if(is_boolean(type))
        {
            return unsigned_to_boolean(value, type);
        }
    }
    if(is_floating(value.type))
    {
        if(is_signed(type))
        {
            return floating_to_signed(value, type);
        }
        if(is_unsigned(type))
        {
            return floating_to_unsigned(value, type);
        }
        if(is_floating(value.type))
        {
            return floating_to_floating(value, type);
        }
    }
    if(is_boolean(value.type))
    {
        if(is_signed(type))
        {
            return boolean_to_signed(value, type);
        }
        if(is_unsigned(type))
        {
            return boolean_to_unsigned(value, type);
        }
        if(is_boolean(type))
        {
            return boolean_to_boolean(value, type);
        }
    }
    auto print_type_a = to_print_type(value.type).begin;
    auto print_type_b = to_print_type(type).begin;
    quit("type cast from '%s' to '%s' not supported", print_type_a, print_type_b);
    return (value_t) {};
}

value_t read_p0();

value_t read_postfix_modify(value_t indirect, str_t operator)
{
    if(str_equal(operator.begin, g_increment))
    {
        read_operator();
        return postfix_increment(indirect);
    }
    if(str_equal(operator.begin, g_decrement))
    {
        read_operator();
        return postfix_decrement(indirect);
    }
    quit("unknown unary operator '%s'", operator.begin);
    return (value_t) {};
}

value_t read_prefix()
{
    auto peek = next_char();
    auto alnum = peek_alnum();
    if(str_equal(alnum.begin, g_new))
    {
        read_alnum();
        auto type = read_type();
        return to_new(type);
    }
    if(str_equal(alnum.begin, g_sizeof))
    {
        read_alnum();
        auto keyword = peek_alnum();
        if(is_type_name(keyword))
        {
            auto type = read_type();
            return to_type_sizeof(type);
        }
        else
        {
            auto value = read_p0();
            return to_sizeof(value);
        }
    }
    if(str_equal(alnum.begin, g_del))
    {
        read_alnum();
        auto value = read_p0();
        return do_del(value);
    }
    auto operator = peek_operator();
    if(str_equal(operator.begin, g_increment))
    {
        read_operator();
        auto value = read_p0();
        return prefix_increment(value);
    }
    if(str_equal(operator.begin, g_decrement))
    {
        read_operator();
        auto value = read_p0();
        return prefix_decrement(value);
    }
    if(peek == *g_less)
    {
        match(g_less);
        auto type = read_type();
        match(g_greater);
        match(g_left_paren);
        auto value = read_expression();
        match(g_rite_paren);
        return type_cast(value, type);
    }
    if(peek == *g_not)
    {
        match(g_not);
        auto value = read_p0();
        return to_not(value);
    }
    if(peek == *g_bitwise_not)
    {
        match(g_bitwise_not);
        auto value = read_p0();
        return to_bitwise_not(value);
    }
    if(peek == *g_add)
    {
        match(g_add);
        auto value = read_p0();
        return to_positive(value);
    }
    if(peek == *g_subtract)
    {
        match(g_subtract);
        auto value = read_p0();
        return to_negative(value);
    }
    if(peek == *g_ampersand)
    {
        match(g_ampersand);
        auto value = read_p0();
        return get_address_of(value);
    }
    if(peek == *g_multiply)
    {
        match(g_multiply);
        auto value = read_p0();
        return dereference(value);
    }
    quit("unknown unary operator '%s'", alnum.begin);
    return (value_t) {};
}

value_t call_function(value_t), field_access(value_t), index_access(value_t);

bool is_increment_decrement(str_t operator)
{
    return str_equal(operator.begin, g_increment)
        || str_equal(operator.begin, g_decrement);
}

value_t read_postfix_access(value_t offset)
{
    auto peek = next_char();
    if(peek == *g_left_paren)
    {
        return call_function(offset);
    }
    if(offset.is_function == false)
    {
        offset = load_indirect(offset);
    }
    if(peek == *g_dot)
    {
        if(offset.is_function && !is_pointer(offset.type))
        {
            quit("expected pointer with '%s' access on type '%s' with direct function access", g_dot, offset.type.name.begin);
        }
        return field_access(offset);
    }
    if(peek == *g_left_square)
    {
        return index_access(offset);
    }
    auto operator = peek_operator();
    if(is_increment_decrement(operator))
    {
        return read_postfix_modify(offset, operator);
    }
    return offset;
}

value_t index_access(value_t indirect)
{
    auto operator = str_init(g_index);
    match(g_left_square);
    auto index = read_expression();
    assert_type(index.type, operator, is_size);
    match(g_rite_square);
    auto array = dereference(indirect);
    auto offset = (value_t) {
        .slot = get_slot(),
        .type = array.type,
    };
    auto llvm_type = to_llvm_type(offset.type).begin;
    emit(g_opcode_gep, offset.slot, llvm_type, array.slot, g_i64, index.slot);
    return read_postfix_access(offset);
}

value_t load_string()
{
    auto value = (value_t) {
        .type.name = str_init(g_i8),
        .slot = get_slot(),
    };
    value.type.stars = 1;
    auto string = read_string();
    auto size = 0;
    auto fixed = fix_escape_chars(string, &size);
    emit(g_opcode_alloca_string, value.slot, size);
    emit(g_opcode_store_string, size, fixed.begin, value.slot);
    if(next_char() == *g_left_square)
    {
        return index_access(value);
    }
    return value;
}

value_t field_index(value_t found, str_t name)
{
    if(is_pointer(found.type))
    {
        found = dereference(found);
    }
    auto type = value_in_list(found.type.name, &g_file.types);
    auto exists = str_in_list(name, &type->names);
    if(type == nullptr || exists == nullptr)
    {
        quit("could not access field '%s' in type '%s'", name.begin, found.type.name.begin);
    }
    auto index = exists - type->names.begin;
    auto offset = (value_t) {
        .slot = get_slot(),
        .type = type->types.begin[index],
        .is_lvalue = true,
    };
    auto llvm_type = to_llvm_type(found.type).begin;
    emit(g_opcode_type_field, offset.slot, llvm_type, found.slot, index);
    return offset;
}

value_t field_access(value_t found)
{
    match(g_dot);
    auto name = read_alnum();
    auto offset = field_index(found, name);
    return read_postfix_access(offset);
}

value_t call_function(value_t found)
{
    auto types = (type_list_t) {};
    auto slots = read_function_call_arg_list(&types);
    if(found.unlock_args == false)
    {
        if(types.size != found.types.size)
        {
            quit("function '%s' expected '%d' args but got '%d' args", found.name.begin, found.types.size, types.size);
        }
        auto operator = str_init(g_function);
        for(auto i = 0; i < slots.size; i++)
        {
            auto type = types.begin[i];
            auto expected = found.types.begin[i];
            assert_types_match(type, expected, operator);
        }
    }
    auto value = (value_t) {
        .slot = get_slot(),
        .type = found.type,
        .is_function = true,
    };
    auto llvm_type = to_llvm_type(found.type).begin;
    str_equal(found.type.name.begin, g_void)
        ? emit(g_opcode_void_call, llvm_type, found.name.begin)
        : emit(g_opcode_call, value.slot, llvm_type, found.name.begin);
    emit(g_str, g_left_paren);
    for(auto i = 0; i < slots.size; i++)
    {
        auto slot = slots.begin[i];
        auto llvm_type = to_llvm_type(types.begin[i]).begin;
        emit(g_opcode_type_slot, llvm_type, slot);
        if(i < slots.size - 1)
        {
            emit(g_str, g_comma);
        }
    }
    emit(g_str, g_rite_paren);
    return read_postfix_access(value);
}

value_t read_postfix()
{
    auto alnum = read_alnum();
    auto found = value_in_list(alnum, &g_file.values);
    if(!found)
    {
        quit("'%s' not declared", alnum.begin);
    }
    return read_postfix_access(*found);
}

value_t load_character()
{
    match(g_escape_apostrophe);
    auto c = peek_char();
    step();
    if(c == *g_escape_backslash)
    {
        c = convert_escape_char(peek_char());
        step();
    }
    match(g_escape_apostrophe);
    auto value = (value_t) {
        .slot = get_slot(),
        .type.name = str_init(g_i8),
    };
    emit(g_opcode_load_character, g_file.slot, value.type.name.begin, c);
    return value;
}

value_t assignment_operate(value_t left, value_t rite, str_t operator)
{
    assert_lvalue(left, operator);
    auto llvm_type = to_llvm_type(rite.type).begin;
    emit(g_opcode_store, llvm_type, rite.slot, left.slot);
    return to_rvalue(left);
}

value_t operate(value_t, value_t, str_t);

value_t aggregate_operate(value_t left, value_t rite, str_t operator)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = left.type,
    };
    out.type.stars = 0;
    out.is_lvalue = true;
    out = alloca_value(out);
    if(is_relational(operator))
    {
        out.type.name = str_init(g_i1);
    }
    if(is_boolean(out.type))
    {
        auto print_type = to_print_type(left.type).begin;
        quit("aggregate type '%s' does not support boolean operator '%s'", print_type, operator.begin);
    }
    auto members = value_in_list(left.type.name, &g_file.types);
    for(auto i = 0; i < members->names.size; i++)
    {
        auto name = members->names.begin[i];
        auto left_member = field_index(left, name);
        auto rite_member = field_index(rite, name);
        auto result = operate(left_member, rite_member, operator);
        auto assignment = str_init(g_equals);
        auto temp = field_index(out, name);
        operate(temp, result, assignment);
    }
    return out;
}

value_t emit_operation(value_t out, value_t left, value_t rite, str_t operator, char* format)
{
    if(format)
    {
        auto llvm_type = to_llvm_type(left.type).begin;
        emit(format, out.slot, llvm_type, left.slot, rite.slot);
        return out;
    }
    auto print_type_a = to_print_type(left.type).begin;
    auto print_type_b = to_print_type(rite.type).begin;
    quit("unknown operator '%s' on types '%s' and '%s'", operator.begin, print_type_a, print_type_b);
    return (value_t) {};
}

value_t floating_operate(value_t left, value_t rite, str_t operator)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = left.type,
    };
    out.type.stars = 0;
    if(is_relational(operator))
    {
        out.type.name = str_init(g_i1);
    }
    return emit_operation(out, left, rite, operator,
        str_equal(operator.begin, g_multiply        ) ? g_opcode_floating_mul              :
        str_equal(operator.begin, g_divide          ) ? g_opcode_floating_div              :
        str_equal(operator.begin, g_add             ) ? g_opcode_floating_add              :
        str_equal(operator.begin, g_subtract        ) ? g_opcode_floating_sub              :
        str_equal(operator.begin, g_equal_to        ) ? g_opcode_floating_equal_to         :
        str_equal(operator.begin, g_not_equal_to    ) ? g_opcode_floating_not_equal_to     :
        str_equal(operator.begin, g_less            ) ? g_opcode_floating_less             :
        str_equal(operator.begin, g_less_equal_to   ) ? g_opcode_floating_less_equal_to    :
        str_equal(operator.begin, g_greater         ) ? g_opcode_floating_greater          :
        str_equal(operator.begin, g_greater_equal_to) ? g_opcode_floating_greater_equal_to :
        nullptr
    );
}

value_t unsigned_operate(value_t left, value_t rite, str_t operator)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = left.type,
    };
    out.type.stars = 0;
    if(is_relational(operator))
    {
        out.type.name = str_init(g_i1);
    }
    return emit_operation(out, left, rite, operator,
        str_equal(operator.begin, g_multiply        ) ? g_opcode_unsigned_mul              :
        str_equal(operator.begin, g_divide          ) ? g_opcode_unsigned_divide           :
        str_equal(operator.begin, g_mod             ) ? g_opcode_unsigned_remainder        :
        str_equal(operator.begin, g_add             ) ? g_opcode_unsigned_add              :
        str_equal(operator.begin, g_subtract        ) ? g_opcode_unsigned_sub              :
        str_equal(operator.begin, g_equal_to        ) ? g_opcode_unsigned_equal_to         :
        str_equal(operator.begin, g_not_equal_to    ) ? g_opcode_unsigned_not_equal_to     :
        str_equal(operator.begin, g_less            ) ? g_opcode_unsigned_less             :
        str_equal(operator.begin, g_less_equal_to   ) ? g_opcode_unsigned_less_equal_to    :
        str_equal(operator.begin, g_greater         ) ? g_opcode_unsigned_greater          :
        str_equal(operator.begin, g_greater_equal_to) ? g_opcode_unsigned_greater_equal_to :
        str_equal(operator.begin, g_bitwise_and     ) ? g_opcode_unsigned_bitwise_and      :
        str_equal(operator.begin, g_bitwise_or      ) ? g_opcode_unsigned_bitwise_or       :
        str_equal(operator.begin, g_bitwise_xor     ) ? g_opcode_unsigned_bitwise_xor      :
        str_equal(operator.begin, g_shift_left      ) ? g_opcode_unsigned_shift_left       :
        str_equal(operator.begin, g_shift_rite      ) ? g_opcode_unsigned_shift_rite       :
        nullptr
    );
}

value_t signed_operate(value_t left, value_t rite, str_t operator)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = left.type,
    };
    out.type.stars = 0;
    if(is_relational(operator))
    {
        out.type.name = str_init(g_i1);
    }
    return emit_operation(out, left, rite, operator,
        str_equal(operator.begin, g_multiply        ) ? g_opcode_signed_mul              :
        str_equal(operator.begin, g_divide          ) ? g_opcode_signed_divide           :
        str_equal(operator.begin, g_mod             ) ? g_opcode_signed_remainder        :
        str_equal(operator.begin, g_add             ) ? g_opcode_signed_add              :
        str_equal(operator.begin, g_subtract        ) ? g_opcode_signed_sub              :
        str_equal(operator.begin, g_equal_to        ) ? g_opcode_signed_equal_to         :
        str_equal(operator.begin, g_not_equal_to    ) ? g_opcode_signed_not_equal_to     :
        str_equal(operator.begin, g_less            ) ? g_opcode_signed_less             :
        str_equal(operator.begin, g_less_equal_to   ) ? g_opcode_signed_less_equal_to    :
        str_equal(operator.begin, g_greater         ) ? g_opcode_signed_greater          :
        str_equal(operator.begin, g_greater_equal_to) ? g_opcode_signed_greater_equal_to :
        str_equal(operator.begin, g_bitwise_and     ) ? g_opcode_signed_bitwise_and      :
        str_equal(operator.begin, g_bitwise_or      ) ? g_opcode_signed_bitwise_or       :
        str_equal(operator.begin, g_bitwise_xor     ) ? g_opcode_signed_bitwise_xor      :
        str_equal(operator.begin, g_shift_left      ) ? g_opcode_signed_shift_left       :
        str_equal(operator.begin, g_shift_rite      ) ? g_opcode_signed_shift_rite       :
        nullptr
    );
}

value_t pointer_operate(value_t left, value_t rite, str_t operator)
{
    auto out = (value_t) {
        .slot = get_slot(),
        .type = left.type,
    };
    out.type.stars = 0;
    if(is_relational(operator))
    {
        out.type.name = str_init(g_i1);
    }
    return emit_operation(out, left, rite, operator,
        str_equal(operator.begin, g_equal_to        ) ? g_opcode_signed_equal_to         :
        str_equal(operator.begin, g_not_equal_to    ) ? g_opcode_signed_not_equal_to     :
        str_equal(operator.begin, g_less            ) ? g_opcode_signed_less             :
        str_equal(operator.begin, g_less_equal_to   ) ? g_opcode_signed_less_equal_to    :
        str_equal(operator.begin, g_greater         ) ? g_opcode_signed_greater          :
        str_equal(operator.begin, g_greater_equal_to) ? g_opcode_signed_greater_equal_to :
        nullptr
    );
}

value_t operate(value_t left, value_t rite, str_t operator)
{
    assert_types_match(left.type, rite.type, operator);
    if(is_assignment_operator(operator))
    {
        rite = to_rvalue(rite);
        return assignment_operate(left, rite, operator);
    }
    else
    {
        if(is_aggregate(left.type))
        {
            assert_lvalue(left, operator);
            assert_lvalue(rite, operator);
            left = load_indirect(left);
            rite = load_indirect(rite);
            return aggregate_operate(left, rite, operator);
        }
        else
        {
            left = to_rvalue(left);
            rite = to_rvalue(rite);
            if(is_floating(left.type))
            {
                return floating_operate(left, rite, operator);
            }
            if(is_unsigned(left.type))
            {
                return unsigned_operate(left, rite, operator);
            }
            if(is_signed(left.type) || is_boolean(left.type))
            {
                return signed_operate(left, rite, operator);
            }
            if(is_pointer(left.type))
            {
                return pointer_operate(left, rite, operator);
            }
        }
    }
    quit("unknown type name %s\n", left.type.name.begin);
    return (value_t) {};
}

value_t read_ltor(value_t with(), precedence_t precedence)
{
    auto operators = get_operators(precedence);
    auto left = with();
    while(str_in(peek_operator(), operators))
    {
        auto operator = read_operator();
        auto rite = with();
        left = operate(left, rite, operator);
    }
    return left;
}

value_t read_rtol(value_t with(), precedence_t precedence)
{
    auto operators = get_operators(precedence);
    auto left = with();
    if(str_in(peek_operator(), operators))
    {
        auto operator = read_operator();
        auto rite = read_rtol(with, precedence);
        left = operate(left, rite, operator);
    }
    return to_rvalue(left);
}

value_t read_grouped_expression()
{
    match(g_left_paren);
    auto value = read_expression();
    match(g_rite_paren);
    return value;
}

value_t read_p0()
{
    auto peek = next_char();
    if(peek == *g_apostrophe)
    {
        return load_character();
    }
    if(peek == *g_escape_quotation)
    {
        return load_string();
    }
    if(is_digit_char(peek))
    {
        return load_direct();
    }
    if(is_alpha_char(peek) && !str_in(peek_alnum(), g_construct_keywords))
    {
        return read_postfix();
    }
    if(peek == *g_left_paren)
    {
        return read_grouped_expression();
    }
    return read_prefix();
}

value_t read_p1()
{
    return read_ltor(read_p0, g_precedence_arithmetic_0);
}

value_t read_p2()
{
    return read_ltor(read_p1, g_precedence_arithmetic_1);
}

value_t read_p3()
{
    return read_ltor(read_p2, g_precedence_shift);
}

value_t read_p4()
{
    return read_ltor(read_p3, g_precedence_relational_0);
}

value_t read_p5()
{
    return read_ltor(read_p4, g_precedence_relational_1);
}

value_t read_p6()
{
    return read_ltor(read_p5, g_precedence_bitwise_and);
}

value_t read_p7()
{
    return read_ltor(read_p6, g_precedence_bitwise_xor);
}

value_t read_p8()
{
    return read_ltor(read_p7, g_precedence_bitwise_or);
}

value_t read_p9()
{
    return read_rtol(read_p8, g_precedence_assignment);
}

value_t read_expression()
{
    return read_p9();
}

void read_code(str_t path)
{
    auto fp = fopen(path.begin, "r");
    if(fp == nullptr)
    {
        quit("could not open '%s'", path.begin);
    }
    auto max = code_last_index();
    get_code()->path = path;
    get_code()->line = 1;
    get_code()->size = fread(get_code()->begin, sizeof(char), max, fp);
    fclose(fp);
    if(get_code()->size == max)
    {
        quit("file size exceeds compiler buffer");
    }
}

void push_code(str_t path)
{
    g_file.module += 1;
    read_code(path);
}

void pop_code()
{
    auto code = get_code();
    fprintf(stderr, "%scompiled%s %s: %d bytes\n", g_green, g_normal, code->path.begin, code->size);
    *get_code() = (code_t) {};
    g_file.module -= 1;
}

void read_include()
{
    read_alnum();
    auto path = read_string();
    if(path.size == 0)
    {
        quit("expected path");
    }
    match(g_semicolon);
    push_code(path);
}

void write_header()
{
    emit(g_opcode_target);
    emit(g_str, g_empty);
}

void read_program()
{
    write_header();
    while(true)
    {
        if(is_end_of_file())
        {
            if(g_file.module == 0)
            {
                break;
            }
            else
            {
                pop_code();
            }
        }
        auto alnum = peek_alnum();
        if(str_equal(alnum.begin, g_type))
        {
            read_type_def();
        }
        else
        if(str_equal(alnum.begin, g_include))
        {
            read_include();
        }
        else
        {
            read_function();
        }
    }
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        return 1;
    }
    read_code(str_init(argv[1]));
    read_program();
}
