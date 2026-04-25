#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define len(x) (sizeof(x) / sizeof(*x))

constexpr size_t g_str_size = 64;
constexpr size_t g_value_list_size = 1024;
constexpr size_t g_slot_list_size = 8;
constexpr size_t g_defer_list_size = 8;
constexpr size_t g_type_list_size = 8;
constexpr size_t g_code_size = 65536;
constexpr size_t g_max_operators_per_precedence = 8;

typedef char chars_t[g_str_size];

typedef enum : size_t
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
    size_t begin[g_slot_list_size];
    size_t size;
}
slot_list_t;

typedef enum : size_t
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
    size_t at;
    size_t block;
}
defer_t;

typedef struct
{
    defer_t begin[g_defer_list_size];
    size_t size;
}
defer_list_t;

typedef struct
{
    chars_t begin;
    size_t size;
}
str_t;

typedef struct
{
    str_t name;
    size_t stars;
}
type_t;

typedef struct
{
    type_t begin[g_type_list_size];
    size_t size;
}
type_list_t;

typedef struct
{
    type_t type;
    str_t name;
    bool is_lvalue;
    size_t slot;
    type_list_t types;
}
value_t;

typedef struct
{
    value_t begin[g_value_list_size];
    size_t size;
}
value_list_t;

typedef struct
{
    char begin[g_code_size];
    size_t size;
    size_t at;
}
code_t;

typedef struct
{
    code_t code;
    defer_list_t defers;
    slot_list_t loop_again;
    slot_list_t loop_end;
    value_list_t values;
    size_t line;
    size_t block;
    size_t slot;
    size_t tabs;
    size_t label;
}
file_t;

chars_t g_red                     = "\033[31m";
chars_t g_white                   = "\033[1;37m";
chars_t g_normal                  = "\033[0m";
chars_t g_underscore              = "_";
chars_t g_left_square             = "[";
chars_t g_rite_square             = "]";
chars_t g_left_curl               = "{";
chars_t g_rite_curl               = "}";
chars_t g_left_paren              = "(";
chars_t g_rite_paren              = ")";
chars_t g_semicolon               = ";";
chars_t g_newline                 = "\n";
chars_t g_space                   = " ";
chars_t g_tab                     = "\t";
chars_t g_lower_begin             = "a";
chars_t g_lower_end               = "z";
chars_t g_upper_begin             = "A";
chars_t g_upper_end               = "Z";
chars_t g_digit_begin             = "0";
chars_t g_digit_end               = "9";
chars_t g_not                     = "!";
chars_t g_type_cast               = "<>";
chars_t g_function                = "()";
chars_t g_index                   = "[]";
chars_t g_add                     = "+";
chars_t g_subtract                = "-";
chars_t g_divide                  = "/";
chars_t g_multiply                = "*";
chars_t g_equals                  = "=";
chars_t g_equal_to                = "==";
chars_t g_not_equal_to            = "!=";
chars_t g_less_equal_to           = "<=";
chars_t g_less                    = "<";
chars_t g_greater_equal_to        = ">=";
chars_t g_greater                 = ">";
chars_t g_shift_rite              = ">>";
chars_t g_shift_left              = "<<";
chars_t g_bitwise_or              = "|";
chars_t g_bitwise_xor             = "^";
chars_t g_bitwise_and             = "&";
chars_t g_bitwise_not             = "~";
chars_t g_comma                   = ",";
chars_t g_ampersand               = "&";
chars_t g_void                    = "void";
chars_t g_i1                      = "i1";
chars_t g_i8                      = "i8";
chars_t g_i16                     = "i16";
chars_t g_i32                     = "i32";
chars_t g_i64                     = "i64";
chars_t g_ret                     = "ret";
chars_t g_ptr                     = "ptr";
chars_t g_if                      = "if";
chars_t g_else                    = "else";
chars_t g_while                   = "while";
chars_t g_break                   = "break";
chars_t g_continue                = "continue";
chars_t g_defer                   = "defer";
chars_t g_new                     = "new";
chars_t g_del                     = "del";
chars_t g_sizeof                  = "sizeof";
chars_t g_opcode_label            = "L%lu:";
chars_t g_opcode_branch_if_else   = "br i1 %%%llu, label %%L%lu, label %%L%lu";
chars_t g_opcode_branch           = "br label %%L%lu";
chars_t g_opcode_target           = "target triple = \"x86_64-pc-linux-gnu\"";
chars_t g_opcode_not              = "%%%lu = xor %s %%%lu, true";
chars_t g_opcode_negative         = "%%%lu = mul %s %%%lu, -1";
chars_t g_opcode_mul              = "%%%lu = mul %s %%%lu, %%%lu";
chars_t g_opcode_sdiv             = "%%%lu = sdiv %s %%%lu, %%%lu";
chars_t g_opcode_add              = "%%%lu = add %s %%%lu, %%%lu";
chars_t g_opcode_sub              = "%%%lu = sub %s %%%lu, %%%lu";
chars_t g_opcode_equal_to         = "%%%lu = icmp eq %s %%%lu, %%%lu";
chars_t g_opcode_not_equal_to     = "%%%lu = icmp ne %s %%%lu, %%%lu";
chars_t g_opcode_less             = "%%%lu = icmp slt %s %%%lu, %%%lu";
chars_t g_opcode_less_equal_to    = "%%%lu = icmp sle %s %%%lu, %%%lu";
chars_t g_opcode_greater          = "%%%lu = icmp sgt %s %%%lu, %%%lu";
chars_t g_opcode_greater_equal_to = "%%%lu = icmp sge %s %%%lu, %%%lu";
chars_t g_opcode_bitwise_or       = "%%%lu = or %s %%%lu, %%%lu";
chars_t g_opcode_bitwise_not      = "%%%lu = xor %s %%%lu, -1";
chars_t g_opcode_bitwise_xor      = "%%%lu = xor %s %%%lu, %%%lu";
chars_t g_opcode_bitwise_and      = "%%%lu = and %s %%%lu, %%%lu";
chars_t g_opcode_shift_left       = "%%%lu = shl %s %%%lu, %%%lu";
chars_t g_opcode_shift_rite       = "%%%lu = ashr %s %%%lu, %%%lu";
chars_t g_opcode_alloca           = "%%%lu = alloca %s";
chars_t g_opcode_flat_gep         = "%%%lu = getelementptr ptr, ptr %%%lu, %s %lu";
chars_t g_opcode_gep              = "%%%lu = getelementptr %s, ptr %%%lu, %s %%%lu";
chars_t g_opcode_sizeof           = "%%%lu = getelementptr %s, ptr null, i64 1";
chars_t g_opcode_ptr_to_int       = "%%%lu = ptrtoint ptr %%%llu to %s";
chars_t g_opcode_int_to_ptr       = "%%%lu = inttoptr %s %%%llu to ptr";
chars_t g_opcode_load_immediate   = "%%%lu = add %s %s, 0";
chars_t g_opcode_ret              = "ret %s %%%lu";
chars_t g_opcode_ret_void         = "ret %s";
chars_t g_opcode_define           = "define %s @%s";
chars_t g_opcode_declare          = "declare %s @%s";
chars_t g_opcode_load             = "%%%lu = load %s, ptr %%%lu";
chars_t g_opcode_store            = "store %s %%%lu, ptr %%%lu";
chars_t g_opcode_zero_init        = "store %s zeroinitializer, ptr %%%lu";
chars_t g_opcode_type             = "%s %%%lu";
chars_t g_opcode_call             = "%%%lu = call %s @%s";
chars_t g_opcode_void_call        = "call %s @%s";
chars_t g_opcode_entry            = "entry:";
chars_t g_opcode_signed_extend    = "%%%lu = sext %s %%%lu to %s";
chars_t g_opcode_trunc            = "%%%lu = trunc %s %%%lu to %s";
chars_t g_opcode_malloc           = "%%%llu = call ptr @malloc(i64 %%%llu)";
chars_t g_opcode_free             = "call void @free(ptr %%%llu)";

char* g_operator_chars[] = {
    g_not,
    g_add,
    g_subtract,
    g_divide,
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
    nullptr
};

char* g_operators_by_precedence[g_precedence_count][g_max_operators_per_precedence] = {
    [ g_precedence_arithmetic_0 ]  = { g_multiply, g_divide                                   },
    [ g_precedence_arithmetic_1 ]  = { g_add, g_subtract                                      },
    [ g_precedence_shift        ]  = { g_shift_left, g_shift_rite                             },
    [ g_precedence_relational_0 ]  = { g_less, g_less_equal_to, g_greater, g_greater_equal_to },
    [ g_precedence_relational_1 ]  = { g_equal_to, g_not_equal_to                             },
    [ g_precedence_bitwise_and  ]  = { g_bitwise_and                                          },
    [ g_precedence_bitwise_xor  ]  = { g_bitwise_xor                                          },
    [ g_precedence_bitwise_or   ]  = { g_bitwise_or                                           },
    [ g_precedence_assignment   ]  = { g_equals                                               },
};

#define list_full(list)                    \
    ((list)->size == len((list)->begin) - 1)

#define list_append(file, list, value) do  \
{                                          \
    if(list_full(list))                    \
        quit(file, "list overflow");       \
    (list)->begin[(list)->size++] = value; \
}                                          \
while(false)

bool is_digit_char(char c)
{
    return c >= *g_digit_begin && c <= *g_digit_end;
}

bool is_not_semicolon(char c)
{
    return c != *g_semicolon;
}

bool is_lower_char(char c)
{
    return c >= *g_lower_begin && c <= *g_lower_end;
}

bool is_upper_char(char c)
{
    return c >= *g_upper_begin && c <= *g_upper_end;
}

bool is_alpha_char(char c)
{
    return is_lower_char(c) || is_upper_char(c);
}

bool is_alnum_char(char c)
{
    return is_alpha_char(c) || is_digit_char(c) || c == *g_underscore;
}

bool is_space_char(char c)
{
    return c == *g_space || c == *g_newline || c == *g_tab;
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

void code_rewind(code_t* code, size_t by)
{
    code->at -= by;
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
    return type.stars > 0 || is_generic_pointer(type);
}

bool is_scalar(type_t type)
{
    return type.stars == 0;
}

void quit(file_t* file, char* format, ...)
{
    auto out = stderr;
    va_list args = {};
    va_start(args, format);
    fprintf(out, "%sline %lu:%s %serror: %s", g_white, file->line, g_normal, g_red, g_normal);
    vfprintf(out, format, args);
    fprintf(out, g_newline);
    va_end(args);
    exit(1);
}

void emit(file_t* file, char* format, ...)
{
    auto out = stdout;
    va_list args = {};
    va_start(args, format);
    for(size_t tab = 0; tab < file->tabs; tab++)
    {
        fprintf(out, g_tab);
    }
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, g_newline);
}

str_t str_init(file_t* file, chars_t chars)
{
    str_t str = {};
    while(*chars)
    {
        list_append(file, &str, *chars);
        chars += 1;
    }
    return str;
}

str_t to_llvm_type(file_t* file, type_t type)
{
    if(is_pointer(type))
    {
        return str_init(file, g_ptr);
    }
    else
    {
        return type.name;
    }
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

value_t* value_in(str_t str, value_list_t* list)
{
    for(size_t i = 0; i < list->size; i++)
    {
        auto value = &list->begin[i];
        if(str_equal(str.begin, value->name.begin))
        {
            return value;
        }
    }
    return nullptr;
}

bool is_reserved_keyword(str_t keyword)
{
    return str_in(keyword, g_type_keywords)
        || str_in(keyword, g_control_keywords)
        || str_in(keyword, g_construct_keywords);
}

size_t get_block(file_t* file)
{
    file->block += 1;
    return file->block;
}

size_t get_slot(file_t* file)
{
    file->slot += 1;
    return file->slot;
}

size_t get_label(file_t* file)
{
    file->label += 1;
    return file->label;
}

file_t file_init(char* path)
{
    file_t file = {};
    file.line = 1;
    auto fp = fopen(path, "r");
    if(fp == nullptr)
    {
        quit(&file, "could not open '%s'", path);
    }
    file.code.size = fread(file.code.begin, sizeof(char), g_code_size - 1, fp),
    fclose(fp);
    return file;
}

char peek_char(file_t* file)
{
    return file->code.begin[file->code.at];
}

void step(file_t* file)
{
    file->code.at += 1;
    if(file->code.at == g_code_size - 1)
    {
        quit(file, "unexpected end of file");
    }
}

void assert_types_match(file_t* file, type_t left, type_t rite, str_t operator)
{
    if(left.stars != rite.stars)
    {
        quit(file, "pointer level mismatch (%llu and %llu) with '%s'", left.stars, rite.stars, operator.begin);
    }
    if(!str_equal(left.name.begin, rite.name.begin))
    {
        quit(file, "types '%s' and '%s' mismatch with '%s'", left.name.begin, rite.name.begin, operator.begin);
    }
}

void assert_scalar(file_t* file, type_t left, type_t rite, str_t operator)
{
    assert_types_match(file, left, rite, operator);
    if(is_pointer(left) || is_pointer(rite))
    {
        quit(file, "expected scalars with '%s'", operator.begin);
    }
}

void assert_scalar_of(file_t* file, value_t value, chars_t type, str_t operator)
{
    type_t expect = {
        .name = str_init(file, type)
    };
    assert_scalar(file, value.type, expect, operator);
}

void assert_scalar_any(file_t* file, value_t value, str_t operator)
{
    if(str_in(value.type.name, g_scalars))
    {
        assert_scalar(file, value.type, value.type, operator);
        return;
    }
    quit(file, "%s does not support operator %s", value.type.name.begin, operator.begin);
}

void assert_lvalue(file_t* file, value_t left, str_t operator)
{
    if(!left.is_lvalue)
    {
        quit(file, "expected lvalue with '%s'", operator.begin);
    }
}

size_t skip_space(file_t* file)
{
    size_t count = 0;
    while(true)
    {
        auto c = peek_char(file);
        if(is_space_char(c))
        {
            if(c == *g_newline)
            {
                file->line += 1;
            }
            step(file);
            count += 1;
        }
        else
        {
            break;
        }
    }
    return count;
}

char next_char(file_t* file)
{
    skip_space(file);
    return peek_char(file);
}

bool is_end_of_file(file_t* file)
{
    skip_space(file);
    return file->code.at == file->code.size;
}

void match(file_t* file, char* expected)
{
    auto size = strlen(expected);
    str_t got = {};
    for(size_t i = 0; i < size; i++)
    {
        list_append(file, &got, next_char(file));
        step(file);
    }
    if(!str_equal(got.begin, expected))
    {
        quit(file, "expected %s, got %s", expected, got.begin);
    }
}

str_t read_chars(file_t* file, bool matches(char))
{
    skip_space(file);
    str_t str = {};
    while(true)
    {
        auto c = peek_char(file);
        if(matches(c))
        {
            list_append(file, &str, c);
            step(file);
        }
        else
        {
            break;
        }
    }
    return str;
}

str_t read_operator(file_t* file)
{
    return read_chars(file, is_operator_char);
}

str_t read_alnum(file_t* file)
{
    return read_chars(file, is_alnum_char);
}

str_t read_digit(file_t* file)
{
    return read_chars(file, is_digit_char);
}

str_t read_till_semicolon(file_t* file)
{
    return read_chars(file, is_not_semicolon);
}

str_t peek_operator(file_t* file)
{
    auto operator = read_operator(file);
    code_rewind(&file->code, operator.size);
    return operator;
}

str_t peek_alnum(file_t* file)
{
    auto alnum = read_alnum(file);
    code_rewind(&file->code, alnum.size);
    return alnum;
}

size_t read_stars(file_t* file)
{
    size_t stars = 0;
    while(true)
    {
        if(next_char(file) == *g_multiply)
        {
            stars += 1;
            step(file);
        }
        else
        {
            break;
        }
    }
    return stars;
}

type_t read_type(file_t* file)
{
    type_t type = {
        .name = read_alnum(file),
        .stars = read_stars(file),
    };
    return type;
}

value_t read_value(file_t* file)
{
    value_t value = {
        .type = read_type(file),
        .name = read_alnum(file),
        .slot = get_slot(file),
    };
    return value;
}

value_t read_value_decl(file_t* file)
{
    auto value = read_value(file);
    if(value_in(value.name, &file->values))
    {
        quit(file, "'%s' already declared", value.name.begin);
    }
    if(is_reserved_keyword(value.name))
    {
        quit(file, "'%s' is a reserved keyword", value.name.begin);
    }
    if(!str_in(value.type.name, g_type_keywords))
    {
        quit(file, "'%s' not a valid type", value.type.name.begin);
    }
    return value;
}

value_list_t read_function_decl_arg_list(file_t* file)
{
    value_list_t values = {};
    match(file, g_left_paren);
    while(true)
    {
        if(next_char(file) == *g_rite_paren)
        {
            break;
        }
        auto arg = read_value_decl(file);
        list_append(file, &values, arg);
        if(next_char(file) == *g_comma)
        {
            match(file, g_comma);
            if(next_char(file) == *g_rite_paren)
            {
                quit(file, "expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(file, g_rite_paren);
    return values;
}

value_t read_expression(file_t*);

void execute_defers(file_t* file, size_t total)
{
    auto at = file->code.at;
    for(size_t i = 0; i < total; i++)
    {
        file->code.at = file->defers.begin[file->defers.size - 1 - i].at;
        read_expression(file);
    }
    file->code.at = at;
}

value_t read_ret_statement(file_t* file, value_t ret_value)
{
    execute_defers(file, file->defers.size);
    read_alnum(file);
    if(str_equal(ret_value.type.name.begin, g_void))
    {
        match(file, g_semicolon);
        value_t value = {
            .type.name = str_init(file, g_void)
        };
        emit(file, g_opcode_ret_void, g_void);
        return value;
    }
    else
    {
        auto value = read_expression(file);
        match(file, g_semicolon);
        auto llvm_type = to_llvm_type(file, value.type).begin;
        emit(file, g_opcode_ret, llvm_type, value.slot);
        return value;
    }
}

bool read_statement(file_t*, value_t, scope_t, size_t);

void read_if_statement(file_t* file, value_t ret_value, size_t if_label, size_t else_label, size_t end_label, size_t block)
{
    read_alnum(file);
    match(file, g_left_paren);
    auto value = read_expression(file);
    value_t expected = {
        .type.name = str_init(file, g_i1)
    };
    auto operator = str_init(file, g_if);
    assert_types_match(file, value.type, expected.type, operator);
    match(file, g_rite_paren);
    emit(file, g_opcode_branch_if_else, value.slot, if_label, else_label);
    emit(file, g_opcode_label, if_label);
    auto terminated = read_statement(file, ret_value, g_scope_if, block);
    if(!terminated)
    {
        emit(file, g_opcode_branch, end_label);
    }
}

void read_else_statement(file_t* file, value_t ret_value, size_t else_label, size_t end_label, size_t block)
{
    emit(file, g_opcode_label, else_label);
    auto keyword = peek_alnum(file);
    if(str_equal(keyword.begin, g_else))
    {
        read_alnum(file);
        auto terminated = read_statement(file, ret_value, g_scope_if, block);
        if(!terminated)
        {
            emit(file, g_opcode_branch, end_label);
        }
    }
    else
    {
        emit(file, g_opcode_branch, end_label);
    }
}

void read_if_else_statement(file_t* file, value_t ret_value, size_t block)
{
    auto if_label = get_label(file);
    auto else_label = get_label(file);
    auto end_label = get_label(file);
    read_if_statement(file, ret_value, if_label, else_label, end_label, block);
    read_else_statement(file, ret_value, else_label, end_label, block);
    emit(file, g_opcode_label, end_label);
}

void read_defer_statement(file_t* file, scope_t scope, size_t block)
{
    read_alnum(file);
    skip_space(file);
    defer_t defer = {
        .scope = scope,
        .at = file->code.at,
        .block = block
    };
    list_append(file, &file->defers, defer);
    read_till_semicolon(file);
    match(file, g_semicolon);
}

void execute_break_defers(file_t* file)
{
    auto scopes = file->defers.begin;
    size_t defers = 0;
    int at = file->defers.size - 1;
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
        execute_defers(file, defers);
    }
}

void read_break_statement(file_t* file)
{
    read_alnum(file);
    match(file, g_semicolon);
    if(file->loop_end.size == 0)
    {
        quit(file, "%s statement not within a loop", g_break);
    }
    execute_break_defers(file);
    auto to = file->loop_end.begin[file->loop_end.size - 1];
    emit(file, g_opcode_branch, to);
}

void read_continue_statement(file_t* file)
{
    read_alnum(file);
    match(file, g_semicolon);
    if(file->loop_again.size == 0)
    {
        quit(file, "%s statement not within a loop", g_continue);
    }
    execute_break_defers(file);
    auto to = file->loop_again.begin[file->loop_again.size - 1];
    emit(file, g_opcode_branch, to);
}

void read_while_statement(file_t* file, value_t ret_value, size_t block)
{
    auto again_label = get_label(file);
    auto while_label = get_label(file);
    auto end_label = get_label(file);
    list_append(file, &file->loop_again, again_label);
    list_append(file, &file->loop_end, end_label);
    emit(file, g_opcode_branch, again_label);
    emit(file, g_opcode_label, again_label);
    read_alnum(file);
    match(file, g_left_paren);
    auto value = read_expression(file);
    match(file, g_rite_paren);
    value_t expected = {
        .type.name = str_init(file, g_i1)
    };
    auto operator = str_init(file, g_while);
    assert_types_match(file, value.type, expected.type, operator);
    emit(file, g_opcode_branch_if_else, value.slot, while_label, end_label);
    emit(file, g_opcode_label, while_label);
    auto terminated = read_statement(file, ret_value, g_scope_while, block);
    if(!terminated)
    {
        emit(file, g_opcode_branch, again_label);
    }
    emit(file, g_opcode_label, end_label);
    file->loop_again.size -= 1;
    file->loop_end.size -= 1;
}

bool read_block(file_t*, value_t, scope_t);

bool read_statement(file_t* file, value_t ret_value, scope_t scope, size_t block)
{
    auto keyword = peek_alnum(file);
    if(str_in(keyword, g_control_keywords))
    {
        if(str_equal(keyword.begin, g_ret))
        {
            auto operator = str_init(file, g_ret);
            auto value = read_ret_statement(file, ret_value);
            assert_types_match(file, value.type, ret_value.type, operator);
            return true;
        }
        if(str_equal(keyword.begin, g_continue))
        {
            read_continue_statement(file);
            return true;
        }
        if(str_equal(keyword.begin, g_break))
        {
            read_break_statement(file);
            return true;
        }
        if(str_equal(keyword.begin, g_defer))
        {
            read_defer_statement(file, scope, block);
        }
        if(str_equal(keyword.begin, g_if))
        {
            read_if_else_statement(file, ret_value, block);
        }
        if(str_equal(keyword.begin, g_else))
        {
            quit(file, "missing binding '%s'", g_if);
        }
        if(str_equal(keyword.begin, g_while))
        {
            read_while_statement(file, ret_value, block);
        }
    }
    else
    if(str_in(keyword, g_type_keywords))
    {
        auto value = read_value_decl(file);
        list_append(file, &file->values, value);
        auto llvm_type = to_llvm_type(file, value.type).begin;
        emit(file, g_opcode_alloca, value.slot, llvm_type);
        emit(file, g_opcode_zero_init, llvm_type, value.slot);
        auto spaces = skip_space(file);
        if(peek_char(file) == *g_semicolon)
        {
            match(file, g_semicolon);
        }
        else
        {
            code_rewind(&file->code, spaces);
            code_rewind(&file->code, value.name.size);
            read_expression(file);
            match(file, g_semicolon);
        }
    }
    else
    {
        if(next_char(file) == *g_left_curl)
        {
            return read_block(file, ret_value, scope);
        }
        else
        {
            read_expression(file);
            match(file, g_semicolon);
        }
    }
    return false;
}

bool read_block(file_t* file, value_t ret_value, scope_t scope)
{
    auto block = get_block(file);
    auto defers = file->defers.size;
    auto values = file->values.size;
    bool terminated = false;
    file->tabs += 1;
    match(file, g_left_curl);
    while(true)
    {
        if(next_char(file) == *g_rite_curl)
        {
            break;
        }
        if(terminated)
        {
            quit(file, "block was terminated");
        }
        terminated = read_statement(file, ret_value, scope, block);
    }
    if(!terminated)
    {
        execute_defers(file, file->defers.size - defers);
    }
    match(file, g_rite_curl);
    file->tabs -= 1;
    file->defers.size = defers;
    file->values.size = values;
    return terminated;
}

void emit_parameters(file_t* file, value_list_t* args, value_t* ret_value)
{
    emit(file, g_left_paren);
    for(size_t i = 0; i < args->size; i++)
    {
        auto type = args->begin[i].type;
        auto slot = args->begin[i].slot;
        list_append(file, &ret_value->types, type);
        auto llvm_type = to_llvm_type(file, type).begin;
        emit(file, g_opcode_type, llvm_type, slot);
        if(i < args->size - 1)
        {
            emit(file, g_comma);
        }
    }
    emit(file, g_rite_paren);
}

void read_function(file_t* file)
{
    file->slot = 0;
    auto ret_value = read_value_decl(file);
    auto llvm_type = to_llvm_type(file, ret_value.type).begin;
    auto args = read_function_decl_arg_list(file);
    if(next_char(file) == *g_semicolon)
    {
        emit(file, g_opcode_declare, llvm_type, ret_value.name.begin);
        emit_parameters(file, &args, &ret_value);
        list_append(file, &file->values, ret_value);
        match(file, g_semicolon);
    }
    else
    {
        emit(file, g_opcode_define, llvm_type, ret_value.name.begin);
        emit_parameters(file, &args, &ret_value);
        list_append(file, &file->values, ret_value);
        emit(file, g_left_curl);
        emit(file, g_opcode_entry);
        for(size_t i = 0; i < args.size; i++)
        {
            auto arg = args.begin[i];
            auto slot = get_slot(file);
            auto llvm_type = to_llvm_type(file, arg.type).begin;
            emit(file, g_opcode_alloca, slot, llvm_type);
            emit(file, g_opcode_store, llvm_type, arg.slot, slot);
            arg.slot = slot;
            list_append(file, &file->values, arg);
        }
        bool terminated = read_block(file, ret_value, g_scope_function);
        if(!terminated)
        {
            quit(file, "block missing %s statement", g_ret);
        }
        emit(file, g_rite_curl);
        file->values.size -= args.size;
    }
    emit(file, "");
}

void write_header(file_t* file)
{
    emit(file, g_opcode_target);
    emit(file, "");
}

void read_program(file_t* file)
{
    write_header(file);
    while(true)
    {
        if(is_end_of_file(file))
        {
            break;
        }
        read_function(file);
    }
}

value_t to_rvalue(file_t* file, value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = get_slot(file);
        auto llvm_type = to_llvm_type(file, value.type).begin;
        emit(file, g_opcode_load, slot, llvm_type, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

char** get_operators(file_t* file, precedence_t precedence)
{
    if(precedence >= g_precedence_count)
    {
        quit(file, "unknown precedence level");
    }
    return g_operators_by_precedence[precedence];
}

bool is_relational(file_t* file, str_t operator)
{
    return str_in(operator, get_operators(file, g_precedence_relational_0))
        || str_in(operator, get_operators(file, g_precedence_relational_1));
}

value_t operate(file_t* file, value_t left, value_t rite, str_t operator)
{
    if(str_in(operator, get_operators(file, g_precedence_assignment)))
    {
        assert_lvalue(file, left, operator);
        assert_types_match(file, left.type, rite.type, operator);
        rite = to_rvalue(file, rite);
        auto llvm_type = to_llvm_type(file, rite.type).begin;
        emit(file, g_opcode_store, llvm_type, rite.slot, left.slot);
        return to_rvalue(file, left);
    }
    else
    {
        left = to_rvalue(file, left);
        rite = to_rvalue(file, rite);
        assert_types_match(file, left.type, rite.type, operator);
        value_t out = {
            .slot = get_slot(file),
            .type = left.type,
        };
        out.type.stars = 0;
        auto format =
            str_equal( operator.begin, g_multiply         ) ? g_opcode_mul              :
            str_equal( operator.begin, g_divide           ) ? g_opcode_sdiv             :
            str_equal( operator.begin, g_add              ) ? g_opcode_add              :
            str_equal( operator.begin, g_subtract         ) ? g_opcode_sub              :
            str_equal( operator.begin, g_equal_to         ) ? g_opcode_equal_to         :
            str_equal( operator.begin, g_not_equal_to     ) ? g_opcode_not_equal_to     :
            str_equal( operator.begin, g_less             ) ? g_opcode_less             :
            str_equal( operator.begin, g_less_equal_to    ) ? g_opcode_less_equal_to    :
            str_equal( operator.begin, g_greater          ) ? g_opcode_greater          :
            str_equal( operator.begin, g_greater_equal_to ) ? g_opcode_greater_equal_to :
            str_equal( operator.begin, g_bitwise_and      ) ? g_opcode_bitwise_and      :
            str_equal( operator.begin, g_bitwise_or       ) ? g_opcode_bitwise_or       :
            str_equal( operator.begin, g_bitwise_xor      ) ? g_opcode_bitwise_xor      :
            str_equal( operator.begin, g_shift_left       ) ? g_opcode_shift_left       :
            str_equal( operator.begin, g_shift_rite       ) ? g_opcode_shift_rite       : nullptr;
        if(format)
        {
            auto llvm_type = to_llvm_type(file, left.type).begin;
            emit(file, format, out.slot, llvm_type, left.slot, rite.slot);
            if(is_relational(file, operator))
            {
                out.type.name = str_init(file, g_i1);
            }
            return out;
        }
        quit(file, "unknown operator '%s'", operator.begin);
        return (value_t) {};
    }
}

value_t read_ltor(file_t* file, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(file, precedence);
    auto left = with(file);
    while(str_in(peek_operator(file), operators))
    {
        auto operator = read_operator(file);
        auto rite = with(file);
        left = operate(file, left, rite, operator);
    }
    return left;
}

value_t read_rtol(file_t* file, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(file, precedence);
    auto left = with(file);
    if(str_in(peek_operator(file), operators))
    {
        auto operator = read_operator(file);
        auto rite = read_rtol(file, with, precedence);
        left = operate(file, left, rite, operator);
    }
    return to_rvalue(file, left);
}

value_t load_direct(file_t* file)
{
    value_t value = {
        .type.name = str_init(file, g_i64),
        .slot = get_slot(file),
    };
    auto name = read_digit(file);
    emit(file, g_opcode_load_immediate, file->slot, value.type.name.begin, name.begin);
    return value;
}

size_t push_arg(file_t* file, type_t expected)
{
    auto arg = read_expression(file);
    auto operator = str_init(file, g_function);
    assert_types_match(file, arg.type, expected, operator);
    return file->slot;
}

slot_list_t read_function_call_arg_list(file_t* file, value_t* found)
{
    slot_list_t list = {};
    match(file, g_left_paren);
    while(true)
    {
        if(next_char(file) == *g_rite_paren)
        {
            break;
        }
        auto expected = found->types.begin[list.size];
        auto slot = push_arg(file, expected);
        list_append(file, &list, slot);
        if(next_char(file) == *g_comma)
        {
            match(file, g_comma);
            if(next_char(file) == *g_rite_paren)
            {
                quit(file, "expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(file, g_rite_paren);
    return list;
}

value_t call_function(file_t* file, value_t* found)
{
    auto list = read_function_call_arg_list(file, found);
    value_t value = {
        .slot = get_slot(file),
        .type = found->type,
    };
    auto llvm_type = to_llvm_type(file, found->type).begin;
    if(str_equal(found->type.name.begin, g_void))
    {
        emit(file, g_opcode_void_call, llvm_type, found->name.begin);
    }
    else
    {
        emit(file, g_opcode_call, value.slot, llvm_type, found->name.begin);
    }
    emit(file, g_left_paren);
    for(size_t i = 0; i < list.size; i++)
    {
        auto slot = list.begin[i];
        auto llvm_type = to_llvm_type(file, found->types.begin[i]).begin;
        emit(file, g_opcode_type, llvm_type, slot);
        if(i < list.size - 1)
        {
            emit(file, g_comma);
        }
    }
    emit(file, g_rite_paren);
    return value;
}

value_t load_indirect(file_t* file, value_t* found)
{
    value_t value = {
        .is_lvalue = true,
        .slot = get_slot(file),
        .type = found->type,
    };
    emit(file, g_opcode_flat_gep, value.slot, found->slot, g_i64, 0);
    return value;
}

value_t get_address_of(file_t* file, value_t value)
{
    auto operator = str_init(file, g_ampersand);
    assert_lvalue(file, value, operator);
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

value_t dereference(file_t* file, value_t value)
{
    auto operator = str_init(file, g_multiply);
    assert_lvalue(file, value, operator);
    auto slot = get_slot(file);
    auto llvm_type = to_llvm_type(file, value.type).begin;
    emit(file, g_opcode_load, slot, llvm_type, value.slot);
    value.slot = slot;
    value.type.stars -= 1;
    return value;
}

value_t to_positive(file_t* file, value_t value)
{
    auto operator = str_init(file, g_add);
    value = to_rvalue(file, value);
    assert_scalar_any(file, value, operator);
    return value;
}

value_t to_negative(file_t* file, value_t value)
{
    auto operator = str_init(file, g_subtract);
    value = to_rvalue(file, value);
    assert_scalar_any(file, value, operator);
    value_t out = {
        .slot = get_slot(file),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(file, value.type).begin;
    emit(file, g_opcode_negative, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_bitwise_not(file_t* file, value_t value)
{
    auto operator = str_init(file, g_bitwise_not);
    value = to_rvalue(file, value);
    assert_scalar_any(file, value, operator);
    value_t out = {
        .slot = get_slot(file),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(file, value.type).begin;
    emit(file, g_opcode_bitwise_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_not(file_t* file, value_t value)
{
    auto operator = str_init(file, g_not);
    value = to_rvalue(file, value);
    assert_scalar_of(file, value, g_i1, operator);
    value_t out = {
        .slot = get_slot(file),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(file, value.type).begin;
    emit(file, g_opcode_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_sizeof(file_t* file, value_t value)
{
    value = to_rvalue(file, value);
    auto slot = get_slot(file);
    value_t out = {
        .slot = get_slot(file),
        .type.name = str_init(file, g_i64),
    };
    auto llvm_type = to_llvm_type(file, value.type).begin;
    emit(file, g_opcode_sizeof, slot, llvm_type);
    emit(file, g_opcode_ptr_to_int, out.slot, slot, g_i64);
    return out;
}

value_t to_type_sizeof(file_t* file, type_t type)
{
    auto slot = get_slot(file);
    value_t out = {
        .slot = get_slot(file),
        .type.name = str_init(file, g_i64),
    };
    auto llvm_type = to_llvm_type(file, type).begin;
    emit(file, g_opcode_sizeof, slot, llvm_type);
    emit(file, g_opcode_ptr_to_int, out.slot, slot, g_i64);
    return out;
}

value_t to_new(file_t* file, type_t type)
{
    value_t size = to_type_sizeof(file, type);
    if(peek_char(file) == *g_left_square)
    {
        match(file, g_left_square);
        value_t elems = read_expression(file);
        match(file, g_rite_square);
        value_t total = {
            .slot = get_slot(file),
        };
        auto operator = str_init(file, g_index);
        assert_scalar_of(file, elems, g_i64, operator);
        emit(file, g_opcode_mul, total.slot, g_i64, elems.slot, size.slot);
        value_t out = {
            .slot = get_slot(file),
            .type = type,
        };
        out.type.stars += 1;
        emit(file, g_opcode_malloc, out.slot, total.slot);
        return out;
    }
    else
    {
        value_t out = {
            .slot = get_slot(file),
            .type = type,
        };
        out.type.stars += 1;
        emit(file, g_opcode_malloc, out.slot, size.slot);
        return out;
    }
}

value_t do_del(file_t* file, value_t value)
{
    value = to_rvalue(file, value);
	emit(file, g_opcode_free, value.slot);
    value_t out = {
        .type.name = str_init(file, g_void)
    };
    return out;
}

size_t type_power(file_t* file, type_t type)
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
    quit(file, "unknown type '%s'", at);
    return 0;
}

value_t type_cast(file_t* file, value_t value, type_t type)
{
    value = to_rvalue(file, value);
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
            .slot = get_slot(file),
            .type = type,
        };
        if(is_scalar(value.type))
        {
            emit(file, g_opcode_int_to_ptr, out.slot, from, value.slot);
            return out;
        }
    }
    else
    {
        auto x = type_power(file, type);
        auto y = type_power(file, value.type);
        if(x == y)
        {
            return value;
        }
        value_t out = {
            .slot = get_slot(file),
            .type = type,
        };
        if(x > y)
        {
            emit(file, g_opcode_signed_extend, out.slot, from, value.slot, to);
            return out;
        }
        if(x < y)
        {
            emit(file, g_opcode_trunc, out.slot, from, value.slot, to);
            return out;
        }
    }
    quit(file, "could not type cast '%s' to '%s'", from, to);
    return (value_t) {};
}

value_t read_p0(file_t*);

value_t read_unary(file_t* file)
{
    auto peek = next_char(file);
    auto alnum = peek_alnum(file);
    if(str_equal(alnum.begin, g_new))
    {
        read_alnum(file);
        auto type = read_type(file);
        return to_new(file, type);
    }
    if(str_equal(alnum.begin, g_sizeof))
    {
        read_alnum(file);
        auto keyword = peek_alnum(file);
        if(str_in(keyword, g_type_keywords))
        {
            auto type = read_type(file);
            return to_type_sizeof(file, type);
        }
        else
        {
            auto value = read_p0(file);
            return to_sizeof(file, value);
        }
    }
    if(str_equal(alnum.begin, g_del))
    {
        read_alnum(file);
        value_t value = read_p0(file);
        return do_del(file, value);
    }
    if(peek == *g_less)
    {
        match(file, g_less);
        auto type = read_type(file);
        match(file, g_greater);
        match(file, g_left_paren);
        auto value = read_expression(file);
        match(file, g_rite_paren);
        return type_cast(file, value, type);
    }
    if(peek == *g_not)
    {
        match(file, g_not);
        auto value = read_p0(file);
        return to_not(file, value);
    }
    if(peek == *g_bitwise_not)
    {
        match(file, g_bitwise_not);
        auto value = read_p0(file);
        return to_bitwise_not(file, value);
    }
    if(peek == *g_add)
    {
        match(file, g_add);
        auto value = read_p0(file);
        return to_positive(file, value);
    }
    if(peek == *g_subtract)
    {
        match(file, g_subtract);
        auto value = read_p0(file);
        return to_negative(file, value);
    }
    if(peek == *g_ampersand)
    {
        match(file, g_ampersand);
        auto value = read_p0(file);
        return get_address_of(file, value);
    }
    if(peek == *g_multiply)
    {
        match(file, g_multiply);
        auto value = read_p0(file);
        return dereference(file, value);
    }
    quit(file, "compiler error: '%s'", __func__);
    return (value_t) {};
}

value_t return_indirect_offset(file_t* file, value_t indirect)
{
    auto operator = str_init(file, g_index);
    match(file, g_left_square);
    auto index = read_expression(file);
    assert_scalar_of(file, index, g_i64, operator);
    match(file, g_rite_square);
    auto array = dereference(file, indirect);
    value_t offset = {
        .slot = get_slot(file),
        .type = array.type,
        .is_lvalue = true,
    };
    auto llvm_type = to_llvm_type(file, offset.type).begin;
    emit(file, g_opcode_gep, offset.slot, llvm_type, array.slot, g_i64, index.slot);
    return offset;
}

value_t read_p0(file_t* file)
{
    auto peek = next_char(file);
    if(is_digit_char(peek))
    {
        return load_direct(file);
    }
    if(is_alpha_char(peek))
    {
        auto alnum = peek_alnum(file);
        if(str_in(alnum, g_construct_keywords))
        {
            goto unary;
        }
        read_alnum(file);
        auto found = value_in(alnum, &file->values);
        if(!found)
        {
            quit(file, "'%s' not declared", alnum.begin);
        }
        auto peek = next_char(file);
        if(peek == *g_left_paren)
        {
            return call_function(file, found);
        }
        auto indirect = load_indirect(file, found);
        if(peek == *g_left_square)
        {
            return return_indirect_offset(file, indirect);
        }
        return indirect;
    }
    if(peek == *g_left_paren)
    {
        match(file, g_left_paren);
        auto value = read_expression(file);
        match(file, g_rite_paren);
        return value;
    }
unary:
    return read_unary(file);
}

value_t read_p1(file_t* file)
{
    return read_ltor(file, read_p0, g_precedence_arithmetic_0);
}

value_t read_p2(file_t* file)
{
    return read_ltor(file, read_p1, g_precedence_arithmetic_1);
}

value_t read_p3(file_t* file)
{
    return read_ltor(file, read_p2, g_precedence_shift);
}

value_t read_p4(file_t* file)
{
    return read_ltor(file, read_p3, g_precedence_relational_0);
}

value_t read_p5(file_t* file)
{
    return read_ltor(file, read_p4, g_precedence_relational_1);
}

value_t read_p6(file_t* file)
{
    return read_ltor(file, read_p5, g_precedence_bitwise_and);
}

value_t read_p7(file_t* file)
{
    return read_ltor(file, read_p6, g_precedence_bitwise_xor);
}

value_t read_p8(file_t* file)
{
    return read_ltor(file, read_p7, g_precedence_bitwise_or);
}

value_t read_p9(file_t* file)
{
    return read_rtol(file, read_p8, g_precedence_assignment);
}

value_t read_expression(file_t* file)
{
    return read_p9(file);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        return 1;
    }
    auto file = file_init(argv[1]);
    read_program(&file);
}
