#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

constexpr size_t g_str_size = 64;
constexpr size_t g_value_list_size = 1024;
constexpr size_t g_arg_list_size = 8;
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
    size_t begin[g_arg_list_size];
    size_t size;
}
slot_list_t;

typedef struct
{
    type_t begin[g_arg_list_size];
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
    value_list_t values;
    value_list_t loop_again;
    value_list_t loop_end;
    size_t line;
    size_t slot;
    size_t tabs;
    size_t label;
}
file_t;

chars_t g_red                     = "\033[31m";
chars_t g_white                   = "\033[1;37m";
chars_t g_normal                  = "\033[0m";
chars_t g_underscore              = "_";
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
chars_t g_function                = "()";
chars_t g_i1                      = "i1";
chars_t g_i32                     = "i32";
chars_t g_ret                     = "ret";
chars_t g_ptr                     = "ptr";
chars_t g_if                      = "if";
chars_t g_else                    = "else";
chars_t g_while                   = "while";
chars_t g_break                   = "break";
chars_t g_continue                = "continue";
chars_t g_opcode_target           = "target triple = \"x86_64-pc-linux-gnu\"";
chars_t g_label                   = "L%lu:";
chars_t g_branch_if_else          = "br i1 %%%llu, label %%L%lu, label %%L%lu";
chars_t g_branch                  = "br label %%L%lu";
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
chars_t g_opcode_gep              = "%%%lu = getelementptr ptr, ptr %%%lu, %s %lu";
chars_t g_opcode_load_immediate   = "%%%lu = add %s %s, 0";
chars_t g_opcode_ret              = "ret %s %%%d";
chars_t g_opcode_define           = "define %s @%s";
chars_t g_opcode_load             = "%%%lu = load %s, ptr %%%lu";
chars_t g_opcode_store            = "store %s %%%lu, ptr %%%lu";
chars_t g_opcode_ptr              = "ptr %%%lu";
chars_t g_opcode_function_call    = "%%%lu = call %s @%s";
chars_t g_entry                   = "entry:";

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
    g_i1,
    g_i32,
    nullptr
};

char* g_control_keywords[] = {
    g_ret,
    g_if,
    g_else,
    g_while,
    g_break,
    g_continue,
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

bool is_digit_char(char c)
{
    return c >= *g_digit_begin && c <= *g_digit_end;
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

code_t code_init(char* path)
{
    code_t self = {};
    auto fp = fopen(path, "r");
    self.size = fread(self.begin, sizeof(char), g_code_size - 1, fp),
    fclose(fp);
    return self;
}

void code_rewind(code_t* self, size_t by)
{
    self->at -= by;
}

bool type_is_pointer(type_t self)
{
    return self.stars > 0;
}

void quit(file_t* self, char* format, ...)
{
    auto out = stderr;
    va_list args = {};
    va_start(args, format);
    fprintf(out, "%sline %lu:%s %serror: %s", g_white, self->line, g_normal, g_red, g_normal);
    vfprintf(out, format, args);
    fprintf(out, g_newline);
    va_end(args);
    exit(1);
}

void emit(file_t* self, char* format, ...)
{
    auto out = stdout;
    va_list args = {};
    va_start(args, format);
    for(size_t tab = 0; tab < self->tabs; tab++)
    {
        fprintf(out, g_tab);
    }
    vfprintf(out, format, args);
    fprintf(out, g_newline);
    va_end(args);
}

void str_append(file_t* self, str_t* str, char c)
{
    if(str->size == g_str_size - 1)
    {
        quit(self, "str '%s' truncated", str->begin);
    }
    str->begin[str->size++] = c;
}

str_t str_init(file_t* self, chars_t chars)
{
    str_t str = {};
    while(*chars)
    {
        str_append(self, &str, *chars);
        chars += 1;
    }
    return str;
}

str_t to_llvm_type(file_t* self, type_t type)
{
    if(type_is_pointer(type))
    {
        return str_init(self, g_ptr);
    }
    else
    {
        return type.name;
    }
}

bool str_equal(char* self, char* other)
{
    return strcmp(self, other) == 0;
}

bool str_in(str_t self, char** array)
{
    while(*array)
    {
        if(str_equal(self.begin, *array))
        {
            return true;
        }
        array += 1;
    }
    return false;
}

value_t* value_in(str_t self, value_list_t* list)
{
    for(size_t i = 0; i < list->size; i++)
    {
        auto value = &list->begin[i];
        if(str_equal(self.begin, value->name.begin))
        {
            return value;
        }
    }
    return nullptr;
}

void value_list_append(file_t* self, value_list_t* list, value_t value)
{
    if(list->size == g_value_list_size)
    {
        quit(self, "one file supports max %d value identifiers", g_value_list_size);
    }
    list->begin[list->size++] = value;
}

void type_list_append(file_t* self, type_list_t* list, type_t type)
{
    if(list->size == g_arg_list_size)
    {
        quit(self, "functions support max %d args", g_arg_list_size);
    }
    list->begin[list->size++] = type;
}

void slot_list_append(file_t* self, slot_list_t* list, size_t slot)
{
    if(list->size == g_arg_list_size)
    {
        quit(self, "functions support max %s args", g_arg_list_size);
    }
    list->begin[list->size++] = slot;
}

value_t value_list_top(file_t* self, value_list_t* list)
{
    if(list->size == 0)
    {
        quit(self, "vale list underflow");
    }
    return list->begin[list->size - 1];
}

size_t get_slot(file_t* self)
{
    self->slot += 1;
    return self->slot;
}

size_t get_label(file_t* self)
{
    self->label += 1;
    return self->label;
}

file_t file_init(char* path)
{
    file_t self = {};
    self.code = code_init(path);
    self.line = 1;
    return self;
}

char peek_char(file_t* self)
{
    return self->code.begin[self->code.at];
}

void step(file_t* self)
{
    self->code.at += 1;
    if(self->code.at == g_code_size - 1)
    {
        quit(self, "unexpected end of file");
    }
}

void assert_types_match(file_t* self, type_t left, type_t rite, str_t operator)
{
    if(!str_equal(left.name.begin, rite.name.begin))
    {
        quit(self, "types '%s' and '%s' mismatch with '%s'", left.name.begin, rite.name.begin, operator.begin);
    }
}

void assert_stars_match(file_t* self, type_t left, type_t rite, str_t operator)
{
    if(left.stars != rite.stars)
    {
        quit(self, "pointer level mismatch with '%s'", operator.begin);
    }
}

void assert_scalar(file_t* self, type_t left, type_t rite, str_t operator)
{
    assert_types_match(self, left, rite, operator);
    if(type_is_pointer(left) || type_is_pointer(rite))
    {
        quit(self, "expected scalars with '%s'", operator.begin);
    }
}

void assert_lvalue(file_t* self, value_t left, str_t operator)
{
    if(!left.is_lvalue)
    {
        quit(self, "expected lvalue with '%s'", operator.begin);
    }
}

void skip_space(file_t* self)
{
    while(true)
    {
        auto c = peek_char(self);
        if(is_space_char(c))
        {
            if(c == *g_newline)
            {
                self->line += 1;
            }
            step(self);
        }
        else
        {
            break;
        }
    }
}

char next_char(file_t* self)
{
    skip_space(self);
    return peek_char(self);
}

bool is_end_of_file(file_t* self)
{
    skip_space(self);
    return self->code.at == self->code.size;
}

void match(file_t* self, char* expected)
{
    auto size = strlen(expected);
    str_t got = {};
    for(size_t i = 0; i < size; i++)
    {
        str_append(self, &got, next_char(self));
        step(self);
    }
    if(!str_equal(got.begin, expected))
    {
        quit(self, "expected %s, got %s", expected, got.begin);
    }
}

str_t read_chars(file_t* self, bool matches(char))
{
    skip_space(self);
    str_t str = {};
    while(true)
    {
        auto c = peek_char(self);
        if(matches(c))
        {
            str_append(self, &str, c);
            step(self);
        }
        else
        {
            break;
        }
    }
    return str;
}

str_t read_operator(file_t* self)
{
    return read_chars(self, is_operator_char);
}

str_t read_alnum(file_t* self)
{
    return read_chars(self, is_alnum_char);
}

str_t read_digit(file_t* self)
{
    return read_chars(self, is_digit_char);
}

str_t peek_operator(file_t* self)
{
    auto operator = read_operator(self);
    code_rewind(&self->code, operator.size);
    return operator;
}

str_t peek_alnum(file_t* self)
{
    auto alnum = read_alnum(self);
    code_rewind(&self->code, alnum.size);
    return alnum;
}

size_t read_stars(file_t* self)
{
    size_t stars = 0;
    while(true)
    {
        if(next_char(self) == *g_multiply)
        {
            stars += 1;
            step(self);
        }
        else
        {
            break;
        }
    }
    return stars;
}

type_t read_type(file_t* self)
{
    type_t type = {
        .name = read_alnum(self),
        .stars = read_stars(self),
    };
    return type;
}

value_t read_value(file_t* self)
{
    value_t value = {
        .type = read_type(self),
        .name = read_alnum(self),
        .slot = get_slot(self),
    };
    return value;
}

value_t read_value_decl(file_t* self)
{
    auto value = read_value(self);
    if(value_in(value.name, &self->values))
    {
        quit(self, "'%s' already declared", value.name.begin);
    }
    return value;
}

value_list_t read_function_decl_arg_list(file_t* self, value_t* value)
{
    value_list_t values = {};
    match(self, g_left_paren);
    emit(self, g_left_paren);
    while(true)
    {
        if(next_char(self) == *g_rite_paren)
        {
            break;
        }
        auto arg = read_value_decl(self);
        emit(self, g_opcode_ptr, arg.slot);
        value_list_append(self, &values, arg);
        type_list_append(self, &value->types, arg.type);
        if(next_char(self) == *g_comma)
        {
            match(self, g_comma);
            emit(self, g_comma);
            if(next_char(self) == *g_rite_paren)
            {
                quit(self, "expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(self, g_rite_paren);
    emit(self, g_rite_paren);
    return values;
}

value_t read_expression(file_t*);

value_t read_ret_statement(file_t* self)
{
    read_alnum(self);
    auto value = read_expression(self);
    match(self, g_semicolon);
    auto llvm_type = to_llvm_type(self, value.type).begin;
    emit(self, g_opcode_ret, llvm_type, value.slot);
    return value;
}

bool read_statement(file_t*, value_t);

void read_if_statement(file_t* self, value_t ret_value, size_t if_label, size_t else_label, size_t end_label)
{
    read_alnum(self);
    match(self, g_left_paren);
    auto value = read_expression(self);
    value_t expected = {
        .type.name = str_init(self, g_i1)
    };
    auto operator = str_init(self, g_if);
    assert_types_match(self, value.type, expected.type, operator);
    match(self, g_rite_paren);
    emit(self, g_branch_if_else, value.slot, if_label, else_label);
    emit(self, g_label, if_label);
    auto terminated = read_statement(self, ret_value);
    if(!terminated)
    {
        emit(self, g_branch, end_label);
    }
}

void read_else_statement(file_t* self, value_t ret_value, size_t else_label, size_t end_label)
{
    emit(self, g_label, else_label);
    auto keyword = peek_alnum(self);
    if(str_equal(keyword.begin, g_else))
    {
        read_alnum(self);
        auto terminated = read_statement(self, ret_value);
        if(!terminated)
        {
            emit(self, g_branch, end_label);
        }
    }
    else
    {
        emit(self, g_branch, end_label);
    }
}

void read_if_else_statement(file_t* self, value_t ret_value)
{
    auto if_label = get_label(self);
    auto else_label = get_label(self);
    auto end_label = get_label(self);
    read_if_statement(self, ret_value, if_label, else_label, end_label);
    read_else_statement(self, ret_value, else_label, end_label);
    emit(self, g_label, end_label);
}

void read_continue_statement(file_t* self)
{
    read_alnum(self);
    match(self, g_semicolon);
    if(self->loop_again.size == 0)
    {
        quit(self, "%s statement not within a loop", g_continue);
    }
    auto to = value_list_top(self, &self->loop_again);
    emit(self, g_branch, to.slot);
}

void read_break_statement(file_t* self)
{
    read_alnum(self);
    match(self, g_semicolon);
    if(self->loop_end.size == 0)
    {
        quit(self, "%s statement not within a loop", g_break);
    }
    auto to = value_list_top(self, &self->loop_end);
    emit(self, g_branch, to.slot);
}

void read_while_statement(file_t* self, value_t ret_value)
{
    auto again_label = get_label(self);
    auto while_label = get_label(self);
    auto end_label = get_label(self);
    value_list_append(self, &self->loop_again, (value_t) {.slot = again_label});
    value_list_append(self, &self->loop_end, (value_t) {.slot = end_label});
    emit(self, g_branch, again_label);
    emit(self, g_label, again_label);
    read_alnum(self);
    match(self, g_left_paren);
    auto value = read_expression(self);
    match(self, g_rite_paren);
    value_t expected = {
        .type.name = str_init(self, g_i1)
    };
    auto operator = str_init(self, g_while);
    assert_types_match(self, value.type, expected.type, operator);
    emit(self, g_branch_if_else, value.slot, while_label, end_label);
    emit(self, g_label, while_label);
    auto terminated = read_statement(self, ret_value);
    if(!terminated)
    {
        emit(self, g_branch, again_label);
    }
    emit(self, g_label, end_label);
    self->loop_again.size -= 1;
    self->loop_end.size -= 1;
}

bool read_block(file_t*, value_t);

bool read_statement(file_t* self, value_t ret_value)
{
    auto keyword = peek_alnum(self);
    if(str_in(keyword, g_control_keywords))
    {
        if(str_equal(keyword.begin, g_ret))
        {
            auto operator = str_init(self, g_ret);
            auto value = read_ret_statement(self);
            assert_types_match(self, value.type, ret_value.type, operator);
            assert_stars_match(self, value.type, ret_value.type, operator);
            return true;
        }
        if(str_equal(keyword.begin, g_continue))
        {
            read_continue_statement(self);
            return true;
        }
        if(str_equal(keyword.begin, g_break))
        {
            read_break_statement(self);
            return true;
        }
        if(str_equal(keyword.begin, g_if))
        {
            read_if_else_statement(self, ret_value);
        }
        if(str_equal(keyword.begin, g_else))
        {
            quit(self, "missing binding '%s'", g_if);
        }
        if(str_equal(keyword.begin, g_while))
        {
            read_while_statement(self, ret_value);
        }
    }
    else
    if(str_in(keyword, g_type_keywords))
    {
        auto value = read_value_decl(self);
        value_list_append(self, &self->values, value);
        auto llvm_type = to_llvm_type(self, value.type).begin;
        emit(self, g_opcode_alloca, value.slot, llvm_type);
        match(self, g_semicolon);
    }
    else
    {
        if(next_char(self) == *g_left_curl)
        {
            return read_block(self, ret_value);
        }
        else
        {
            read_expression(self);
            match(self, g_semicolon);
        }
    }
    return false;
}

bool read_block(file_t* self, value_t ret_value)
{
    auto values = self->values.size;
    bool terminated = false;
    self->tabs += 1;
    match(self, g_left_curl);
    while(true)
    {
        if(next_char(self) == *g_rite_curl)
        {
            break;
        }
        if(terminated)
        {
            quit(self, "block was terminated");
        }
        terminated = read_statement(self, ret_value);
    }
    match(self, g_rite_curl);
    self->tabs -= 1;
    self->values.size = values;
    return terminated;
}

void read_function(file_t* self)
{
    self->slot = 0;
    auto ret_value = read_value_decl(self);
    auto llvm_type = to_llvm_type(self, ret_value.type).begin;
    emit(self, g_opcode_define, llvm_type, ret_value.name.begin);
    auto args = read_function_decl_arg_list(self, &ret_value);
    value_list_append(self, &self->values, ret_value);
    for(size_t i = 0; i < args.size; i++)
    {
        value_list_append(self, &self->values, args.begin[i]);
    }
    emit(self, g_left_curl);
    emit(self, g_entry);
    bool terminated = read_block(self, ret_value);
    if(!terminated)
    {
        quit(self, "block missing %s statement", g_ret);
    }
    emit(self, g_rite_curl);
    self->values.size -= args.size;
}

void read_program(file_t* self)
{
    emit(self, g_opcode_target);
    while(true)
    {
        if(is_end_of_file(self))
        {
            break;
        }
        read_function(self);
    }
}

value_t to_rvalue(file_t* self, value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = get_slot(self);
        auto llvm_type = to_llvm_type(self, value.type).begin;
        emit(self, g_opcode_load, slot, llvm_type, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

char** get_operators(file_t* self, precedence_t precedence)
{
    if(precedence >= g_precedence_count)
    {
        quit(self, "unknown precedence level");
    }
    return g_operators_by_precedence[precedence];
}

bool is_relational(file_t* self, str_t operator)
{
    return str_in(operator, get_operators(self, g_precedence_relational_0))
        || str_in(operator, get_operators(self, g_precedence_relational_1));
}

value_t operate(file_t* self, value_t left, value_t rite, str_t operator)
{
    assert_types_match(self, left.type, rite.type, operator);
    if(str_in(operator, get_operators(self, g_precedence_assignment)))
    {
        assert_stars_match(self, left.type, rite.type, operator);
        assert_lvalue(self, left, operator);
        rite = to_rvalue(self, rite);
        auto llvm_type = to_llvm_type(self, rite.type).begin;
        emit(self, g_opcode_store, llvm_type, rite.slot, left.slot);
        return to_rvalue(self, left);
    }
    else
    {
        assert_scalar(self, left.type, rite.type, operator);
        left = to_rvalue(self, left);
        rite = to_rvalue(self, rite);
        value_t out = {
            .slot = get_slot(self),
            .type = left.type,
        };
        auto format =
            str_equal(operator.begin, g_multiply)         ? g_opcode_mul              :
            str_equal(operator.begin, g_divide)           ? g_opcode_sdiv             :
            str_equal(operator.begin, g_add)              ? g_opcode_add              :
            str_equal(operator.begin, g_subtract)         ? g_opcode_sub              :
            str_equal(operator.begin, g_equal_to)         ? g_opcode_equal_to         :
            str_equal(operator.begin, g_not_equal_to)     ? g_opcode_not_equal_to     :
            str_equal(operator.begin, g_less)             ? g_opcode_less             :
            str_equal(operator.begin, g_less_equal_to)    ? g_opcode_less_equal_to    :
            str_equal(operator.begin, g_greater)          ? g_opcode_greater          :
            str_equal(operator.begin, g_greater_equal_to) ? g_opcode_greater_equal_to :
            str_equal(operator.begin, g_bitwise_and)      ? g_opcode_bitwise_and      :
            str_equal(operator.begin, g_bitwise_or)       ? g_opcode_bitwise_or       :
            str_equal(operator.begin, g_bitwise_xor)      ? g_opcode_bitwise_xor      :
            str_equal(operator.begin, g_shift_left)       ? g_opcode_shift_left       :
            str_equal(operator.begin, g_shift_rite)       ? g_opcode_shift_rite       :
            nullptr;
        if(format)
        {
            emit(self, format, out.slot, left.type.name.begin, left.slot, rite.slot);
            if(is_relational(self, operator))
            {
                out.type.name = str_init(self, g_i1);
            }
            return out;
        }
        quit(self, "unknown operator '%s'", operator.begin);
        return (value_t) {};
    }
}

value_t read_ltor(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(self, precedence);
    auto left = with(self);
    while(str_in(peek_operator(self), operators))
    {
        auto operator = read_operator(self);
        auto rite = with(self);
        left = operate(self, left, rite, operator);
    }
    return left;
}

value_t read_rtol(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(self, precedence);
    auto left = with(self);
    if(str_in(peek_operator(self), operators))
    {
        auto operator = read_operator(self);
        auto rite = read_rtol(self, with, precedence);
        left = operate(self, left, rite, operator);
    }
    return to_rvalue(self, left);
}

value_t load_direct(file_t* self)
{
    value_t value = {
        .type.name = str_init(self, g_i32),
        .slot = get_slot(self),
    };
    auto name = read_digit(self);
    emit(self, g_opcode_load_immediate, self->slot, value.type.name.begin, name.begin);
    return value;
}

size_t push_arg(file_t* self, type_t expected)
{
    auto arg = read_expression(self);
    auto operator = str_init(self, g_function);
    assert_types_match(self, arg.type, expected, operator);
    assert_stars_match(self, arg.type, expected, operator);
    auto slot = get_slot(self);
    emit(self, g_opcode_alloca, slot, g_ptr);
    auto llvm_type = to_llvm_type(self, arg.type).begin;
    emit(self, g_opcode_store, llvm_type, arg.slot, slot);
    return slot;
}

slot_list_t read_function_call_arg_list(file_t* self, value_t* found)
{
    slot_list_t list = {};
    match(self, g_left_paren);
    while(true)
    {
        if(next_char(self) == *g_rite_paren)
        {
            break;
        }
        auto expected = found->types.begin[list.size];
        auto slot = push_arg(self, expected);
        slot_list_append(self, &list, slot);
        if(next_char(self) == *g_comma)
        {
            match(self, g_comma);
            if(next_char(self) == *g_rite_paren)
            {
                quit(self, "expected arg");
            }
        }
        else
        {
            break;
        }
    }
    match(self, g_rite_paren);
    return list;
}

value_t call_function(file_t* self, value_t* found)
{
    auto list = read_function_call_arg_list(self, found);
    value_t value = {
        .slot = get_slot(self),
        .type = found->type,
    };
    auto llvm_type = to_llvm_type(self, found->type).begin;
    emit(self, g_opcode_function_call, value.slot, llvm_type, found->name.begin);
    emit(self, g_left_paren);
    for(size_t i = 0; i < list.size; i++)
    {
        emit(self, g_opcode_ptr, list.begin[i]);
        if(i < list.size - 1)
        {
            emit(self, g_comma);
        }
    }
    emit(self, g_rite_paren);
    return value;
}

value_t load_indirect(file_t* self, value_t* found)
{
    value_t value = {
        .is_lvalue = true,
        .slot = get_slot(self),
        .type = found->type,
    };
    emit(self, g_opcode_gep, value.slot, found->slot, g_i32, 0);
    return value;
}

value_t get_address_of(file_t* self, value_t value)
{
    auto operator = str_init(self, g_ampersand);
    assert_lvalue(self, value, operator);
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

value_t dereference(file_t* self, value_t value)
{
    auto operator = str_init(self, g_multiply);
    assert_lvalue(self, value, operator);
    auto slot = get_slot(self);
    auto llvm_type = to_llvm_type(self, value.type).begin;
    emit(self, g_opcode_load, slot, llvm_type, value.slot);
    value.slot = slot;
    value.type.stars -= 1;
    return value;
}

value_t to_positive(file_t* self, value_t value)
{
    auto operator = str_init(self, g_add);
    value = to_rvalue(self, value);
    type_t expect = {
        .name = str_init(self, g_i32)
    };
    assert_scalar(self, value.type, expect, operator);
    return value;
}

value_t to_negative(file_t* self, value_t value)
{
    auto operator = str_init(self, g_subtract);
    value = to_rvalue(self, value);
    type_t expect = {
        .name = str_init(self, g_i32)
    };
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(self, value.type).begin;
    emit(self, g_opcode_negative, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_bitwise_not(file_t* self, value_t value)
{
    auto operator = str_init(self, g_bitwise_not);
    value = to_rvalue(self, value);
    type_t expect = {
        .name = str_init(self, g_i32)
    };
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(self, value.type).begin;
    emit(self, g_opcode_bitwise_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t to_not(file_t* self, value_t value)
{
    auto operator = str_init(self, g_not);
    value = to_rvalue(self, value);
    type_t expect = {
        .name = str_init(self, g_i1)
    };
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    auto llvm_type = to_llvm_type(self, value.type).begin;
    emit(self, g_opcode_not, out.slot, llvm_type, value.slot);
    return out;
}

value_t read_p0(file_t*);

value_t read_unary(file_t* self)
{
    auto peek = next_char(self);
    if(peek == *g_not)
    {
        match(self, g_not);
        auto value = read_p0(self);
        return to_not(self, value);
    }
    if(peek == *g_bitwise_not)
    {
        match(self, g_bitwise_not);
        auto value = read_p0(self);
        return to_bitwise_not(self, value);
    }
    if(peek == *g_add)
    {
        match(self, g_add);
        auto value = read_p0(self);
        return to_positive(self, value);
    }
    if(peek == *g_subtract)
    {
        match(self, g_subtract);
        auto value = read_p0(self);
        return to_negative(self, value);
    }
    if(peek == *g_ampersand)
    {
        match(self, g_ampersand);
        auto value = read_p0(self);
        return get_address_of(self, value);
    }
    if(peek == *g_multiply)
    {
        match(self, g_multiply);
        auto value = read_p0(self);
        return dereference(self, value);
    }
    quit(self, "compiler error: '%s'", __func__);
    return (value_t) {};
}

value_t read_p0(file_t* self)
{
    auto peek = next_char(self);
    if(is_digit_char(peek))
    {
        return load_direct(self);
    }
    if(is_alpha_char(peek))
    {
        auto alnum = read_alnum(self);
        auto found = value_in(alnum, &self->values);
        if(!found)
        {
            quit(self, "'%s' not declared", alnum.begin);
        }
        if(next_char(self) == *g_left_paren)
        {
            return call_function(self, found);
        }
        else
        {
            return load_indirect(self, found);
        }
    }
    if(peek == *g_left_paren)
    {
        match(self, g_left_paren);
        auto value = read_expression(self);
        match(self, g_rite_paren);
        return value;
    }
    return read_unary(self);
}

value_t read_p1(file_t* self) { return read_ltor(self, read_p0, g_precedence_arithmetic_0); }
value_t read_p2(file_t* self) { return read_ltor(self, read_p1, g_precedence_arithmetic_1); }
value_t read_p3(file_t* self) { return read_ltor(self, read_p2, g_precedence_shift       ); }
value_t read_p4(file_t* self) { return read_ltor(self, read_p3, g_precedence_relational_0); }
value_t read_p5(file_t* self) { return read_ltor(self, read_p4, g_precedence_relational_1); }
value_t read_p6(file_t* self) { return read_ltor(self, read_p5, g_precedence_bitwise_and ); }
value_t read_p7(file_t* self) { return read_ltor(self, read_p6, g_precedence_bitwise_xor ); }
value_t read_p8(file_t* self) { return read_ltor(self, read_p7, g_precedence_bitwise_or  ); }
value_t read_p9(file_t* self) { return read_rtol(self, read_p8, g_precedence_assignment  ); }

value_t read_expression(file_t* self)
{
    return read_p9(self);
}

int main()
{
    auto file = file_init("test.n");
    read_program(&file);
}
