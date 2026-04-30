#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define len(x) (sizeof(x) / sizeof(*x))

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

char* const g_quotation               = "\"";
char* const g_str                     = "%s";
char* const g_red                     = "\033[31m";
char* const g_white                   = "\033[1;37m";
char* const g_normal                  = "\033[0m";
char* const g_underscore              = "_";
char* const g_left_square             = "[";
char* const g_rite_square             = "]";
char* const g_left_curl               = "{";
char* const g_rite_curl               = "}";
char* const g_left_paren              = "(";
char* const g_rite_paren              = ")";
char* const g_semicolon               = ";";
char* const g_newline                 = "\n";
char* const g_space                   = " ";
char* const g_tab                     = "\t";
char* const g_lower_begin             = "a";
char* const g_lower_end               = "z";
char* const g_upper_begin             = "A";
char* const g_upper_end               = "Z";
char* const g_digit_begin             = "0";
char* const g_digit_end               = "9";
char* const g_not                     = "!";
char* const g_type_cast               = "<>";
char* const g_function                = "()";
char* const g_index                   = "[]";
char* const g_dot                     = ".";
char* const g_add                     = "+";
char* const g_subtract                = "-";
char* const g_divide                  = "/";
char* const g_mod                     = "%";
char* const g_escape                  = "\\";
char* const g_multiply                = "*";
char* const g_equals                  = "=";
char* const g_equal_to                = "==";
char* const g_not_equal_to            = "!=";
char* const g_less_equal_to           = "<=";
char* const g_less                    = "<";
char* const g_greater_equal_to        = ">=";
char* const g_greater                 = ">";
char* const g_shift_rite              = ">>";
char* const g_shift_left              = "<<";
char* const g_increment               = "++";
char* const g_decrement               = "--";
char* const g_bitwise_or              = "|";
char* const g_bitwise_xor             = "^";
char* const g_bitwise_and             = "&";
char* const g_bitwise_not             = "~";
char* const g_comma                   = ",";
char* const g_ampersand               = "&";
char* const g_percent                 = "%";
char* const g_hash                    = "#";
char* const g_empty                   = "";
char* const g_void                    = "void";
char* const g_i1                      = "i1";
char* const g_i8                      = "i8";
char* const g_i16                     = "i16";
char* const g_i32                     = "i32";
char* const g_i64                     = "i64";
char* const g_double                  = "double";
char* const g_ret                     = "ret";
char* const g_ptr                     = "ptr";
char* const g_if                      = "if";
char* const g_else                    = "else";
char* const g_while                   = "while";
char* const g_break                   = "break";
char* const g_continue                = "continue";
char* const g_type                    = "type";
char* const g_defer                   = "defer";
char* const g_new                     = "new";
char* const g_del                     = "del";
char* const g_sizeof                  = "sizeof";
char* const g_comment                 = "//";
char* const g_include                 = "include";
char* const g_opcode_type_def         = "%%%s = type";
char* const g_opcode_label            = "L%d:";
char* const g_opcode_branch_if_else   = "br i1 %%%d, label %%L%d, label %%L%d";
char* const g_opcode_branch           = "br label %%L%d";
char* const g_opcode_target           = "target triple = \"x86_64-pc-linux-gnu\"";
char* const g_opcode_not              = "%%%d = xor %s %%%d, true";
char* const g_opcode_negative         = "%%%d = mul %s %%%d, -1";
char* const g_opcode_mul              = "%%%d = mul %s %%%d, %%%d";
char* const g_opcode_sdiv             = "%%%d = sdiv %s %%%d, %%%d";
char* const g_opcode_srem             = "%%%d = srem %s %%%d, %%%d";
char* const g_opcode_add              = "%%%d = add %s %%%d, %%%d";
char* const g_opcode_sub              = "%%%d = sub %s %%%d, %%%d";
char* const g_opcode_equal_to         = "%%%d = icmp eq %s %%%d, %%%d";
char* const g_opcode_not_equal_to     = "%%%d = icmp ne %s %%%d, %%%d";
char* const g_opcode_less             = "%%%d = icmp slt %s %%%d, %%%d";
char* const g_opcode_less_equal_to    = "%%%d = icmp sle %s %%%d, %%%d";
char* const g_opcode_greater          = "%%%d = icmp sgt %s %%%d, %%%d";
char* const g_opcode_greater_equal_to = "%%%d = icmp sge %s %%%d, %%%d";
char* const g_opcode_bitwise_or       = "%%%d = or %s %%%d, %%%d";
char* const g_opcode_bitwise_not      = "%%%d = xor %s %%%d, -1";
char* const g_opcode_bitwise_xor      = "%%%d = xor %s %%%d, %%%d";
char* const g_opcode_bitwise_and      = "%%%d = and %s %%%d, %%%d";
char* const g_opcode_shift_left       = "%%%d = shl %s %%%d, %%%d";
char* const g_opcode_shift_rite       = "%%%d = ashr %s %%%d, %%%d";
char* const g_opcode_alloca           = "%%%d = alloca %s";
char* const g_opcode_flat_gep         = "%%%d = getelementptr ptr, ptr %%%d, %s %d";
char* const g_opcode_gep              = "%%%d = getelementptr %s, ptr %%%d, %s %%%d";
char* const g_opcode_sizeof           = "%%%d = getelementptr %s, ptr null, i64 1";
char* const g_opcode_type_field       = "%%%d = getelementptr inbounds %s, ptr %%%d, i32 0, i32 %d";
char* const g_opcode_ptr_to_int       = "%%%d = ptrtoint ptr %%%d to %s";
char* const g_opcode_int_to_ptr       = "%%%d = inttoptr %s %%%d to ptr";
char* const g_opcode_load_double      = "%%%d = fadd %s %s, 0.0";
char* const g_opcode_load_signed      = "%%%d = add %s %s, 0";
char* const g_opcode_ret              = "ret %s %%%d";
char* const g_opcode_ret_void         = "ret %s";
char* const g_opcode_define           = "define %s @%s";
char* const g_opcode_declare          = "declare %s @%s";
char* const g_opcode_load             = "%%%d = load %s, ptr %%%d";
char* const g_opcode_store            = "store %s %%%d, ptr %%%d";
char* const g_opcode_zero_init        = "store %s zeroinitializer, ptr %%%d";
char* const g_opcode_type_slot        = "%s %%%d";
char* const g_opcode_call             = "%%%d = call %s @%s";
char* const g_opcode_void_call        = "call %s @%s";
char* const g_opcode_entry            = "entry:";
char* const g_opcode_signed_extend    = "%%%d = sext %s %%%d to %s";
char* const g_opcode_trunc            = "%%%d = trunc %s %%%d to %s";
char* const g_opcode_malloc           = "%%%d = call ptr @malloc(i64 %%%d)";
char* const g_opcode_free             = "call void @free(ptr %%%d)";
char* const g_opcode_alloca_string    = "%%%d = alloca [%d x i8]";
char* const g_opcode_store_string     = "store [%d x i8] c\"%s\", ptr %%%d";
char* const g_opcode_increment        = "%%%d = add %s %%%d, 1";
char* const g_opcode_decrement        = "%%%d = sub %s %%%d, 1";

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

char* g_type_keywords[] = {
    g_void,
    g_i1,
    g_i8,
    g_i16,
    g_i32,
    g_i64,
    g_double,
    g_ptr,
    nullptr
};

char* g_scalars[] = {
    g_i8,
    g_i16,
    g_i32,
    g_i64,
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

#define list_full(list) ((list)->size == len((list)->begin) - 1)

#define list_append(list, value) if(list_full(list)) quit("list overflow"); else (list)->begin[(list)->size++] = value

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

bool is_string_char(char c)
{
    return is_alpha_char(c)
        || c == *g_space
        || c == *g_underscore
        || c == *g_escape
        || c == *g_dot
        || c == *g_mod
        || c == *g_divide;
}

bool is_space_char(char c)
{
    return c == *g_space
        || c == *g_newline
        || c == *g_tab;
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

bool is_generic_pointer(type_t type)
{
    return str_equal(type.name.begin, g_ptr);
}

bool is_pointer(type_t type)
{
    return type.stars > 0
        || is_generic_pointer(type);
}

bool is_scalar(type_t type)
{
    return type.stars == 0;
}

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
    va_list args = {};
    va_start(args, format);
    fprintf(out, "%s%s: line %d:%s %serror: %s", g_white, get_code()->path.begin, get_code()->line, g_normal, g_red, g_normal);
    vfprintf(out, format, args);
    fprintf(out, g_newline);
    va_end(args);
    exit(1);
}

void emit(char* format, ...)
{
    auto out = stdout;
    va_list args = {};
    va_start(args, format);
    for(int tab = 0; tab < g_file.tabs; tab++)
    {
        fprintf(out, g_tab);
    }
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, g_newline);
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
    str_t str = {};
    str_append(&str, chars);
    return str;
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

char* get_builtin_prefix(type_t type)
{
    return str_in(type.name, g_type_keywords)
        ? g_empty
        : g_percent;
}

str_t to_llvm_type(type_t type)
{
    if(is_pointer(type))
    {
        return str_init(g_ptr);
    }
    else
    {
        auto prefix = get_builtin_prefix(type);
        auto out = str_init(prefix);
        str_append(&out, type.name.begin);
        return out;
    }
}

value_t* value_in_list(str_t str, value_list_t* list)
{
    for(int i = 0; i < list->size; i++)
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
    for(int i = 0; i < list->size; i++)
    {
        auto check = &list->begin[i];
        if(str_equal(str.begin, check->begin))
        {
            return check;
        }
    }
    return nullptr;
}

bool is_type(str_t type)
{
    bool is_builtin = str_in(type, g_type_keywords);
    bool is_type = false;
    for(int i = 0; i < g_file.types.size; i++)
    {
        if(str_equal(type.begin, g_file.types.begin[i].name.begin))
        {
            is_type = true;
            break;
        }
    }
    return is_builtin || is_type;
}

bool is_reserved_keyword(str_t keyword)
{
    return is_type(keyword)
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

void step()
{
    get_code()->at += 1;
    if(get_code()->at == g_code_size - 1)
    {
        quit("unexpected end of file");
    }
}

void assert_types_match(type_t left, type_t rite, str_t operator)
{
    if(left.stars != rite.stars)
    {
        quit("pointer level mismatch (%s %llu and %s %llu) with '%s'", left.name.begin, left.stars, rite.name.begin, rite.stars, operator.begin);
    }
    if(!str_equal(left.name.begin, rite.name.begin))
    {
        quit("types '%s' and '%s' mismatch with '%s'", left.name.begin, rite.name.begin, operator.begin);
    }
}

void assert_scalar(type_t left, type_t rite, str_t operator)
{
    assert_types_match(left, rite, operator);
    if(is_pointer(left) || is_pointer(rite))
    {
        quit("expected scalars with '%s'", operator.begin);
    }
}

void assert_pointer(type_t type, str_t operator)
{
    if(!is_pointer(type))
    {
        quit("expected pointer with '%s'", operator.begin);
    }
}

void assert_scalar_of(type_t type, char* typename, str_t operator)
{
    type_t expect = {
        .name = str_init(typename)
    };
    assert_scalar(type, expect, operator);
}

void assert_scalar_any(type_t type, str_t operator)
{
    if(!str_in(type.name, g_scalars))
    {
        quit("%s does not support operator %s", type.name.begin, operator.begin);
    }
    assert_scalar(type, type, operator);
}

void assert_lvalue(value_t left, str_t operator)
{
    if(!left.is_lvalue)
    {
        quit("expected lvalue with '%s'", operator.begin);
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
            if(c == *g_newline)
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
            while(peek_char() != *g_newline)
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
        quit("expected %s, got %s", expected, got.begin);
    }
}

str_t read_chars(bool matches(char))
{
    str_t str = {};
    while(true)
    {
        auto c = peek_char();
        if(matches(c))
        {
            list_append(&str, c);
            step();
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
    return read_chars(is_string_char);
}

str_t read_numeric()
{
    skip_space_and_comment();
    return read_chars(is_numeric_char);
}

str_t read_till_semicolon()
{
    skip_space_and_comment();
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
    type_t type = {
        .name = read_alnum(),
        .stars = read_stars(),
    };
    return type;
}

value_t read_value()
{
    value_t value = {
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
    if(!is_type(value.type.name))
    {
        quit("'%s' not a valid type", value.type.name.begin);
    }
    return value;
}

value_list_t read_function_decl_arg_list()
{
    value_list_t values = {};
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
    execute_defers(g_file.defers.size);
    read_alnum();
    if(str_equal(ret_value.type.name.begin, g_void))
    {
        match(g_semicolon);
        value_t value = {
            .type.name = str_init(g_void)
        };
        emit(g_opcode_ret_void, g_void);
        return value;
    }
    else
    {
        auto value = read_expression();
        match(g_semicolon);
        auto llvm_type = to_llvm_type(value.type).begin;
        emit(g_opcode_ret, llvm_type, value.slot);
        return value;
    }
}

bool read_statement(value_t, scope_t, int);

typedef struct
{
    int if_label;
    int else_label;
    int end_label;
    int block;
}
branch_t;

void read_if_statement(value_t ret_value, branch_t branch)
{
    read_alnum();
    match(g_left_paren);
    auto value = read_expression();
    value_t expected = {
        .type.name = str_init(g_i1)
    };
    auto operator = str_init(g_if);
    assert_types_match(value.type, expected.type, operator);
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
    branch_t branch = {
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
    defer_t defer = {
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
    value_t expected = {
        .type.name = str_init(g_i1)
    };
    auto operator = str_init(g_while);
    assert_types_match(value.type, expected.type, operator);
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
    if(is_type(keyword))
    {
        auto value = read_value_decl();
        list_append(&g_file.values, value);
        auto llvm_type = to_llvm_type(value.type).begin;
        emit(g_opcode_alloca, value.slot, llvm_type);
        emit(g_opcode_zero_init, llvm_type, value.slot);
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
            quit("block missing %s statement", g_ret);
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
    value_t value = {
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
        if(!is_type(type.name))
        {
            quit("'%s' is not a valid type in aggregate type '%s'", type.name.begin, type_name.begin);
        }
        if(str_in_list(name, &value.names))
        {
            quit("'%s' already a defined member of aggregate type '%s'", name.begin, type_name.begin);
        }
        list_append(&value.types, type);
        list_append(&value.names, name);
        match(g_semicolon);
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

value_t operate(value_t left, value_t rite, str_t operator)
{
    if(str_equal(left.type.name.begin, g_double)
    || str_equal(rite.type.name.begin, g_double))
    {
        quit("floating point operations currently not supported");
    }
    value_t out = {};
    if(str_in(operator, get_operators(g_precedence_assignment)))
    {
        assert_lvalue(left, operator);
        assert_types_match(left.type, rite.type, operator);
        rite = to_rvalue(rite);
        auto llvm_type = to_llvm_type(rite.type).begin;
        emit(g_opcode_store, llvm_type, rite.slot, left.slot);
        out = to_rvalue(left);
    }
    else
    {
        left = to_rvalue(left);
        rite = to_rvalue(rite);
        assert_types_match(left.type, rite.type, operator);
        out.slot = get_slot();
        out.type = left.type;
        out.type.stars = 0;
        auto format =
            str_equal(operator.begin, g_multiply        ) ? g_opcode_mul              :
            str_equal(operator.begin, g_divide          ) ? g_opcode_sdiv             :
            str_equal(operator.begin, g_mod             ) ? g_opcode_srem             :
            str_equal(operator.begin, g_add             ) ? g_opcode_add              :
            str_equal(operator.begin, g_subtract        ) ? g_opcode_sub              :
            str_equal(operator.begin, g_equal_to        ) ? g_opcode_equal_to         :
            str_equal(operator.begin, g_not_equal_to    ) ? g_opcode_not_equal_to     :
            str_equal(operator.begin, g_less            ) ? g_opcode_less             :
            str_equal(operator.begin, g_less_equal_to   ) ? g_opcode_less_equal_to    :
            str_equal(operator.begin, g_greater         ) ? g_opcode_greater          :
            str_equal(operator.begin, g_greater_equal_to) ? g_opcode_greater_equal_to :
            str_equal(operator.begin, g_bitwise_and     ) ? g_opcode_bitwise_and      :
            str_equal(operator.begin, g_bitwise_or      ) ? g_opcode_bitwise_or       :
            str_equal(operator.begin, g_bitwise_xor     ) ? g_opcode_bitwise_xor      :
            str_equal(operator.begin, g_shift_left      ) ? g_opcode_shift_left       :
            str_equal(operator.begin, g_shift_rite      ) ? g_opcode_shift_rite       : nullptr;
        if(format == nullptr)
        {
            quit("unknown operator '%s'", operator.begin);
        }
        auto llvm_type = to_llvm_type(left.type).begin;
        emit(format, out.slot, llvm_type, left.slot, rite.slot);
        if(is_relational(operator))
        {
            out.type.name = str_init(g_i1);
        }
    }
    return out;
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

value_t load_direct()
{
    value_t value = {
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
    str_t out = {};
    for(auto i = 0; i < string.size; i++)
    {
        if(string.begin[i] == '\\')
        {
            switch(string.begin[i + 1])
            {
            case 'n': str_append(&out, "\\0A"); break;
            case 't': str_append(&out, "\\09"); break;
            default : quit("unknown esc char"); break;
            }
            i += 1;
        }
        else
        {
            list_append(&out, string.begin[i]);
        }
        *size += 1;
    }
    str_append(&out, "\\00");
    *size += 1;
    return out;
}

value_t load_string()
{
    match(g_quotation);
    value_t value = {
        .type.name = str_init(g_i8),
        .slot = get_slot(),
    };
    value.type.stars = 1;
    auto string = read_string();
    int size = 0;
    auto fixed = fix_escape_chars(string, &size);
    emit(g_opcode_alloca_string, value.slot, size);
    emit(g_opcode_store_string, size, fixed.begin, value.slot);
    match(g_quotation);
    return value;
}

slot_list_t read_function_call_arg_list(type_list_t* types)
{
    slot_list_t list = {};
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
    assert_scalar_any(value.type, operator);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(out.type).begin;
    emit(g_opcode_increment, out.slot, llvm_type, value.slot);
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
    assert_scalar_any(value.type, operator);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(out.type).begin;
    emit(g_opcode_decrement, out.slot, llvm_type, value.slot);
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

value_t load_indirect(value_t found)
{
    value_t value = {
        .is_lvalue = true,
        .slot = get_slot(),
        .type = found.type,
    };
    emit(g_opcode_flat_gep, value.slot, found.slot, g_i64, 0);
    return value;
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
    auto operator = str_init(g_multiply);
    assert_pointer(value.type, operator);
    if(value.is_lvalue)
    {
        auto slot = get_slot();
        auto llvm_type = to_llvm_type(value.type).begin;
        emit(g_opcode_load, slot, llvm_type, value.slot);
        value.slot = slot;
    }
    value.type.stars -= 1;
    value.is_lvalue = true;
    return value;
}

value_t to_positive(value_t value)
{
    auto operator = str_init(g_add);
    value = to_rvalue(value);
    assert_scalar_any(value.type, operator);
    return value;
}

value_t to_negative(value_t value)
{
    auto operator = str_init(g_subtract);
    value = to_rvalue(value);
    assert_scalar_any(value.type, operator);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_negative, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_bitwise_not(value_t value)
{
    auto operator = str_init(g_bitwise_not);
    value = to_rvalue(value);
    assert_scalar_any(value.type, operator);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_bitwise_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_not(value_t value)
{
    auto operator = str_init(g_not);
    value = to_rvalue(value);
    assert_scalar_of(value.type, g_i1, operator);
    value_t out = {
        .slot = get_slot(),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(value.type).begin;
    emit(g_opcode_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_sizeof(value_t value)
{
    value = to_rvalue(value);
    auto slot = get_slot();
    value_t out = {
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
    value_t out = {
        .slot = get_slot(),
        .type.name = str_init(g_i64),
    };
    auto llvm_type = to_llvm_type(type).begin;
    emit(g_opcode_sizeof, slot, llvm_type);
    emit(g_opcode_ptr_to_int, out.slot, slot, g_i64);
    return out;
}

value_t to_new(type_t type)
{
    value_t size = to_type_sizeof(type);
    if(next_char() == *g_left_square)
    {
        match(g_left_square);
        value_t elems = read_expression();
        match(g_rite_square);
        value_t total = {
            .slot = get_slot(),
        };
        auto operator = str_init(g_index);
        assert_scalar_of(elems.type, g_i64, operator);
        emit(g_opcode_mul, total.slot, g_i64, elems.slot, size.slot);
        value_t out = {
            .slot = get_slot(),
            .type = type,
        };
        out.type.stars += 1;
        emit(g_opcode_malloc, out.slot, total.slot);
        return out;
    }
    else
    {
        value_t out = {
            .slot = get_slot(),
            .type = type,
        };
        out.type.stars += 1;
        emit(g_opcode_malloc, out.slot, size.slot);
        return out;
    }
}

value_t do_del(value_t value)
{
    auto operator = str_init(g_del);
    assert_pointer(value.type, operator);
    value = to_rvalue(value);
	emit(g_opcode_free, value.slot);
    value_t out = {
        .type.name = str_init(g_void)
    };
    return out;
}

int type_power(type_t type)
{
    char* at = type.name.begin;
    if(str_equal(at, g_i1))
    {
        return 0;
    }
    if(str_equal(at, g_i8))
    {
        return 1;
    }
    if(str_equal(at, g_i16))
    {
        return 2;
    }
    if(str_equal(at, g_i32))
    {
        return 3;
    }
    if(str_equal(at, g_i64))
    {
        return 4;
    }
    quit("unknown type '%s'", at);
    return 0;
}

value_t type_cast(value_t value, type_t type)
{
    value = to_rvalue(value);
    auto from = value.type.name.begin;
    auto to = type.name.begin;
    if(is_pointer(type))
    {
        if(is_pointer(value.type))
        {
            value.type = type;
            return value;
        }
        value_t out = {
            .slot = get_slot(),
            .type = type,
        };
        if(is_scalar(value.type))
        {
            emit(g_opcode_int_to_ptr, out.slot, from, value.slot);
            return out;
        }
    }
    else
    {
        auto x = type_power(type);
        auto y = type_power(value.type);
        if(x == y)
        {
            return value;
        }
        value_t out = {
            .slot = get_slot(),
            .type = type,
        };
        if(x > y)
        {
            emit(g_opcode_signed_extend, out.slot, from, value.slot, to);
            return out;
        }
        if(x < y)
        {
            emit(g_opcode_trunc, out.slot, from, value.slot, to);
            return out;
        }
    }
    quit("could not type cast '%s' to '%s'", from, to);
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
        if(is_type(keyword))
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
        value_t value = read_p0();
        return do_del(value);
    }
    auto operator = peek_operator();
    if(str_equal(operator.begin, g_increment))
    {
        read_operator();
        value_t value = read_p0();
        return prefix_increment(value);
    }
    if(str_equal(operator.begin, g_decrement))
    {
        read_operator();
        value_t value = read_p0();
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

value_t call_function(value_t);
value_t field_access(value_t);
value_t index_access(value_t);

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
    assert_scalar_of(index.type, g_i64, operator);
    match(g_rite_square);
    auto array = dereference(indirect);
    value_t offset = {
        .slot = get_slot(),
        .type = array.type,
    };
    auto llvm_type = to_llvm_type(offset.type).begin;
    emit(g_opcode_gep, offset.slot, llvm_type, array.slot, g_i64, index.slot);
    return read_postfix_access(offset);
}

value_t field_access(value_t found)
{
    if(is_pointer(found.type))
    {
        found = dereference(found);
    }
    match(g_dot);
    auto name = read_alnum();
    auto type = value_in_list(found.type.name, &g_file.types);
    auto exists = str_in_list(name, &type->names);
    if(type == nullptr || exists == nullptr)
    {
        quit("could not access field '%s' in type '%s'", name.begin, found.type.name.begin);
    }
    auto index = exists - type->names.begin;
    value_t offset = {
        .slot = get_slot(),
        .type = type->types.begin[index],
    };
    auto llvm_type = to_llvm_type(found.type).begin;
    emit(g_opcode_type_field, offset.slot, llvm_type, found.slot, index);
    return read_postfix_access(offset);
}

value_t call_function(value_t found)
{
    type_list_t types = {};
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
    value_t value = {
        .slot = get_slot(),
        .type = found.type,
        .is_function = true,
    };
    auto llvm_type = to_llvm_type(found.type).begin;
    if(str_equal(found.type.name.begin, g_void))
    {
        emit(g_opcode_void_call, llvm_type, found.name.begin);
    }
    else
    {
        emit(g_opcode_call, value.slot, llvm_type, found.name.begin);
    }
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

value_t read_p0()
{
    auto peek = next_char();
    if(peek == *g_quotation)
    {
        return load_string();
    }
    if(is_digit_char(peek))
    {
        return load_direct();
    }
    if(is_alpha_char(peek))
    {
        if(!str_in(peek_alnum(), g_construct_keywords))
        {
            return read_postfix();
        }
    }
    if(peek == *g_left_paren)
    {
        match(g_left_paren);
        auto value = read_expression();
        match(g_rite_paren);
        return value;
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
    auto max = g_code_size - 1;
    get_code()->path = path;
    get_code()->size = fread(get_code()->begin, sizeof(char), max, fp);
    fprintf(stderr, "%s: %d bytes (max %d bytes)\n", path.begin, get_code()->size, g_code_size);
    fclose(fp);
    if(get_code()->size == max)
    {
        quit("file size exceeds compiler buffer\n");
    }
}

void push_code(str_t path)
{
    g_file.module += 1;
    get_code()->line= 1;
    read_code(path);
}

void pop_code()
{
    *get_code() = (code_t) {};
    g_file.module -= 1;
}

void read_include()
{
    read_alnum();
    match(g_quotation);
    auto path = read_string();
    match(g_quotation);
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
    auto path = argv[1];
    read_code(str_init(path));
    read_program();
}
