#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static constexpr size_t g_string_size = 64;
static constexpr size_t g_value_list_size = 1024;
static constexpr size_t g_arg_list_size = 8;
static constexpr size_t g_code_size = 65536;

typedef char chars_t[g_string_size];

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
string_t;

typedef struct
{
    string_t name;
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
    string_t name;
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

/* Terminal colors */
static constexpr chars_t g_red    = "\033[31m";
static constexpr chars_t g_white  = "\033[1;37m";
static constexpr chars_t g_normal = "\033[0m";

/* Characters */
static constexpr chars_t g_underscore  = "_";
static constexpr chars_t g_left_curl   = "{";
static constexpr chars_t g_rite_curl   = "}";
static constexpr chars_t g_left_paren  = "(";
static constexpr chars_t g_rite_paren  = ")";
static constexpr chars_t g_semicolon   = ";";
static constexpr chars_t g_newline     = "\n";
static constexpr chars_t g_space       = " ";
static constexpr chars_t g_tab         = "\t";
static constexpr chars_t g_lower_begin = "a";
static constexpr chars_t g_lower_end   = "z";
static constexpr chars_t g_upper_begin = "A";
static constexpr chars_t g_upper_end   = "Z";
static constexpr chars_t g_digit_begin = "0";
static constexpr chars_t g_digit_end   = "9";

/* Operators */
static constexpr chars_t g_not                   = "!";
static constexpr chars_t g_add                   = "+";
static constexpr chars_t g_subtract              = "-";
static constexpr chars_t g_divide                = "/";
static constexpr chars_t g_multiply              = "*";
static constexpr chars_t g_equals                = "=";
static constexpr chars_t g_equal_to              = "==";
static constexpr chars_t g_not_equal_to          = "!=";
static constexpr chars_t g_less_than_equal_to    = "<=";
static constexpr chars_t g_less_than             = "<";
static constexpr chars_t g_greater_than_equal_to = ">=";
static constexpr chars_t g_greater_than          = ">";
static constexpr chars_t g_shift_rite            = ">>";
static constexpr chars_t g_shift_left            = "<<";
static constexpr chars_t g_bitwise_or            = "|";
static constexpr chars_t g_bitwise_xor           = "^";
static constexpr chars_t g_bitwise_and           = "&";
static constexpr chars_t g_bitwise_not           = "~";
static constexpr chars_t g_comma                 = ",";
static constexpr chars_t g_ampersand             = "&";
static constexpr chars_t g_function              = "()";

static const char* const g_operator_chars[] = {
    g_not,
    g_add,
    g_subtract,
    g_divide,
    g_multiply,
    g_equals,
    g_equal_to,
    g_not_equal_to,
    g_less_than_equal_to,
    g_less_than,
    g_greater_than_equal_to,
    g_greater_than,
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

/* Types */
static constexpr chars_t g_i1  = "i1";
static constexpr chars_t g_i32 = "i32";

static const char* const g_type_keywords[] = {
    g_i1,
    g_i32,
    nullptr
};

/* Keywords */
static constexpr chars_t g_ret      = "ret";
static constexpr chars_t g_ptr      = "ptr";
static constexpr chars_t g_if       = "if";
static constexpr chars_t g_else     = "else";
static constexpr chars_t g_while    = "while";
static constexpr chars_t g_break    = "break";
static constexpr chars_t g_continue = "continue";

static const char* const g_control_keywords[] = {
    g_ret,
    g_if,
    g_else,
    g_while,
    g_break,
    g_continue,
    nullptr
};

/* Opcodes */
static constexpr chars_t g_opcode_target                = "target triple = \"x86_64-pc-linux-gnu\"";
static constexpr chars_t g_label                        = "L%lu:";
static constexpr chars_t g_branch_if_else               = "br i1 %%%llu, label %%L%lu, label %%L%lu";
static constexpr chars_t g_branch                       = "br label %%L%lu";
static constexpr chars_t g_opcode_not                   = "%%%lu = xor %s %%%lu, true";
static constexpr chars_t g_opcode_negative              = "%%%lu = mul %s %%%lu, -1";
static constexpr chars_t g_opcode_mul                   = "%%%lu = mul %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_sdiv                  = "%%%lu = sdiv %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_add                   = "%%%lu = add %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_sub                   = "%%%lu = sub %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_equal_to              = "%%%lu = icmp eq %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_not_equal_to          = "%%%lu = icmp ne %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_less_than             = "%%%lu = icmp slt %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_less_than_equal_to    = "%%%lu = icmp sle %s %%%lu, %%%lu"; // TODO: UNSIGNED COMPARE
static constexpr chars_t g_opcode_greater_than          = "%%%lu = icmp sgt %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_greater_than_equal_to = "%%%lu = icmp sge %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_bitwise_or            = "%%%lu = or %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_bitwise_not           = "%%%lu = xor %s %%%lu, -1";
static constexpr chars_t g_opcode_bitwise_xor           = "%%%lu = xor %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_bitwise_and           = "%%%lu = and %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_shift_left            = "%%%lu = shl %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_shift_rite            = "%%%lu = ashr %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_alloca                = "%%%lu = alloca %s";
static constexpr chars_t g_opcode_gep                   = "%%%lu = getelementptr ptr, ptr %%%lu, %s %lu";
static constexpr chars_t g_opcode_load_immediate        = "%%%lu = add %s %s, 0";
static constexpr chars_t g_opcode_ret                   = "ret %s %%%d";
static constexpr chars_t g_opcode_define                = "define %s @%s";
static constexpr chars_t g_opcode_load                  = "%%%lu = load %s, ptr %%%lu";
static constexpr chars_t g_opcode_store                 = "store %s %%%lu, ptr %%%lu";
static constexpr chars_t g_opcode_ptr                   = "ptr %%%lu";
static constexpr chars_t g_opcode_function_call         = "%%%lu = call %s @%s";
static constexpr chars_t g_entry                        = "entry:";

static constexpr size_t g_max_operators_per_precedence = 8;

static const char* const g_operators_by_precedence[g_precedence_count][g_max_operators_per_precedence] = {
    [ g_precedence_arithmetic_0 ]  = { g_multiply, g_divide                                                       },
    [ g_precedence_arithmetic_1 ]  = { g_add, g_subtract                                                          },
    [ g_precedence_shift        ]  = { g_shift_left, g_shift_rite                                                 },
    [ g_precedence_relational_0 ]  = { g_less_than, g_less_than_equal_to, g_greater_than, g_greater_than_equal_to },
    [ g_precedence_relational_1 ]  = { g_equal_to, g_not_equal_to                                                 },
    [ g_precedence_bitwise_and  ]  = { g_bitwise_and                                                              },
    [ g_precedence_bitwise_xor  ]  = { g_bitwise_xor                                                              },
    [ g_precedence_bitwise_or   ]  = { g_bitwise_or                                                               },
    [ g_precedence_assignment   ]  = { g_equals                                                                   },
};

static bool
is_digit_char(char c)
{
    return c >= *g_digit_begin
        && c <= *g_digit_end;
}

static bool
is_lower_char(char c)
{
    return c >= *g_lower_begin
        && c <= *g_lower_end;
}

static bool
is_upper_char(char c)
{
    return c >= *g_upper_begin
        && c <= *g_upper_end;
}

static bool
is_alpha_char(char c)
{
    return is_lower_char(c)
        || is_upper_char(c);
}

static bool
is_alnum_char(char c)
{
    return is_alpha_char(c)
        || is_digit_char(c)
        || c == *g_underscore;
}

static bool
is_space_char(char c)
{
    return c == *g_space
        || c == *g_newline
        || c == *g_tab;
}

static bool
is_operator_char(char c)
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

static code_t
code_init(const char* path)
{
    code_t self = {};
    auto fp = fopen(path, "r");
    self.size = fread(self.begin, sizeof(char), g_code_size - 1, fp),
    fclose(fp);
    return self;
}

static void
code_rewind(code_t* self, size_t by)
{
    self->at -= by;
}

static bool
type_is_pointer(type_t self)
{
    return self.stars > 0;
}

static void
quit(file_t* self, const char* format, ...)
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

static void
emit(file_t* self, const char* format, ...)
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

static void
string_append(file_t* self, string_t* string, char c)
{
    if(string->size == g_string_size - 1)
    {
        quit(self, "string '%s' truncated", string->begin);
    }
    string->begin[string->size++] = c;
}

static string_t
string_init(file_t* self, const chars_t chars)
{
    string_t string = {};
    while(*chars)
    {
        string_append(self, &string, *chars);
        chars += 1;
    }
    return string;
}

static string_t
to_llvm_type(file_t* self, type_t type)
{
    if(type_is_pointer(type))
    {
        return string_init(self, g_ptr);
    }
    else
    {
        return type.name;
    }
}

static bool
string_equal(const char* self, const char* other)
{
    return strcmp(self, other) == 0;
}

static bool
string_in(string_t self, const char* const* array)
{
    while(*array)
    {
        if(string_equal(self.begin, *array))
        {
            return true;
        }
        array += 1;
    }
    return false;
}

static value_t*
value_in(string_t self, value_list_t* list)
{
    for(size_t i = 0; i < list->size; i++)
    {
        auto value = &list->begin[i];
        if(string_equal(self.begin, value->name.begin))
        {
            return value;
        }
    }
    return nullptr;
}

static void
value_list_append(file_t* self, value_list_t* list, value_t value)
{
    if(list->size == g_value_list_size)
    {
        quit(self, "one file supports max %d value identifiers", g_value_list_size);
    }
    list->begin[list->size++] = value;
}

static void
type_list_append(file_t* self, type_list_t* list, type_t type)
{
    if(list->size == g_arg_list_size)
    {
        quit(self, "functions support max %d args", g_arg_list_size);
    }
    list->begin[list->size++] = type;
}

static void
slot_list_append(file_t* self, slot_list_t* list, size_t slot)
{
    if(list->size == g_arg_list_size)
    {
        quit(self, "functions support max %s args", g_arg_list_size);
    }
    list->begin[list->size++] = slot;
}

static value_t
value_list_top(file_t* self, value_list_t* list)
{
    if(list->size == 0)
    {
        quit(self, "vale list underflow");
    }
    return list->begin[list->size - 1];
}

static size_t
get_slot(file_t* self)
{
    self->slot += 1;
    return self->slot;
}

static size_t
get_label(file_t* self)
{
    self->label += 1;
    return self->label;
}

static file_t
file_init(const char* path)
{
    file_t self = {};
    self.code = code_init(path);
    self.line = 1;
    return self;
}

static char
peek_char(file_t* self)
{
    return self->code.begin[self->code.at];
}

static void
step(file_t* self)
{
    self->code.at += 1;
    if(self->code.at == g_code_size - 1)
    {
        quit(self, "unexpected end of file");
    }
}

static void
assert_types_match(file_t* self, type_t left, type_t rite, string_t operator)
{
    if(!string_equal(left.name.begin, rite.name.begin))
    {
        quit(self, "types '%s' and '%s' mismatch with '%s'", left.name.begin, rite.name.begin, operator.begin);
    }
}

static void
assert_stars_match(file_t* self, type_t left, type_t rite, string_t operator)
{
    if(left.stars != rite.stars)
    {
        quit(self, "pointer level mismatch with '%s'", operator.begin);
    }
}

static void
assert_scalar(file_t* self, type_t left, type_t rite, string_t operator)
{
    assert_types_match(self, left, rite, operator);
    if(type_is_pointer(left) || type_is_pointer(rite))
    {
        quit(self, "expected scalars with '%s'", operator.begin);
    }
}

static void
assert_lvalue(file_t* self, value_t left, string_t operator)
{
    if(!left.is_lvalue)
    {
        quit(self, "expected lvalue with '%s'", operator.begin);
    }
}

static void
read_space(file_t* self)
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

static void
match(file_t* self, const char* expected)
{
    read_space(self);
    auto size = strlen(expected);
    string_t got = {};
    for(size_t i = 0; i < size; i++)
    {
        string_append(self, &got, peek_char(self));
        step(self);
    }
    if(!string_equal(got.begin, expected))
    {
        quit(self, "expected %s, got %s", expected, got.begin);
    }
}

static string_t
read_while(file_t* self, bool matches(char))
{
    read_space(self);
    string_t string = {};
    while(true)
    {
        auto c = peek_char(self);
        if(matches(c))
        {
            string_append(self, &string, c);
            step(self);
        }
        else
        {
            break;
        }
    }
    return string;
}

static string_t
read_operator(file_t* self)
{
    return read_while(self, is_operator_char);
}

static string_t
read_alnum(file_t* self)
{
    return read_while(self, is_alnum_char);
}

static string_t
read_digit(file_t* self)
{
    return read_while(self, is_digit_char);
}

static string_t
peek_operator(file_t* self)
{
    auto operator = read_operator(self);
    code_rewind(&self->code, operator.size);
    return operator;
}

static string_t
peek_alnum(file_t* self)
{
    auto alnum = read_alnum(self);
    code_rewind(&self->code, alnum.size);
    return alnum;
}

static size_t
read_stars(file_t* self)
{
    size_t stars = 0;
    while(true)
    {
        read_space(self);
        if(peek_char(self) == *g_multiply)
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

static type_t
read_type(file_t* self)
{
    type_t type = {
        .name = read_alnum(self),
        .stars = read_stars(self),
    };
    return type;
}

static value_t
read_value(file_t* self)
{
    value_t value = {
        .type = read_type(self),
        .name = read_alnum(self),
        .slot = get_slot(self),
    };
    return value;
}

static value_t
read_value_decl(file_t* self)
{
    auto value = read_value(self);
    if(value_in(value.name, &self->values))
    {
        quit(self, "'%s' already declared", value.name.begin);
    }
    return value;
}

static value_list_t
read_function_decl_arg_list(file_t* self, value_t* value)
{
    value_list_t values = {};
    match(self, g_left_paren);
    emit(self, g_left_paren);
    while(true)
    {
        read_space(self);
        if(peek_char(self) == *g_rite_paren)
        {
            break;
        }
        auto arg = read_value_decl(self);
        emit(self, g_opcode_ptr, arg.slot);
        value_list_append(self, &values, arg);
        type_list_append(self, &value->types, arg.type);
        read_space(self);
        if(peek_char(self) == *g_comma)
        {
            match(self, g_comma);
            emit(self, g_comma);
            read_space(self);
            if(peek_char(self) == *g_rite_paren)
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

static value_t
read_expression(file_t*);

static value_t
read_ret_statement(file_t* self)
{
    read_alnum(self);
    auto value = read_expression(self);
    match(self, g_semicolon);
    emit(self, g_opcode_ret, to_llvm_type(self, value.type).begin, value.slot);
    return value;
}

static bool
read_block(file_t*, value_t);

static bool
read_statement(file_t*, value_t);

static void
read_if_statement(file_t* self, value_t ret_value, size_t if_label, size_t else_label, size_t end_label)
{
    read_alnum(self);
    match(self, g_left_paren);
    auto value = read_expression(self);
    value_t expected = {
        .type.name = string_init(self, g_i1)
    };
    assert_types_match(self, value.type, expected.type, string_init(self, g_if));
    match(self, g_rite_paren);
    emit(self, g_branch_if_else, value.slot, if_label, else_label);
    emit(self, g_label, if_label);
    auto terminated = read_statement(self, ret_value);
    if(!terminated)
    {
        emit(self, g_branch, end_label);
    }
}

static void
read_else_statement(file_t* self, value_t ret_value, size_t else_label, size_t end_label)
{
    emit(self, g_label, else_label);
    auto keyword = peek_alnum(self);
    if(string_equal(keyword.begin, g_else))
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

static void
read_if_else_statement(file_t* self, value_t ret_value)
{
    auto if_label = get_label(self);
    auto else_label = get_label(self);
    auto end_label = get_label(self);
    read_if_statement(self, ret_value, if_label, else_label, end_label);
    read_else_statement(self, ret_value, else_label, end_label);
    emit(self, g_label, end_label);
}

static void
read_continue_statement(file_t* self)
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

static void
read_break_statement(file_t* self)
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

static void
read_while_statement(file_t* self, value_t ret_value)
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
        .type.name = string_init(self, g_i1)
    };
    auto operator = string_init(self, g_while);
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

static bool
read_statement(file_t* self, value_t ret_value)
{
    auto keyword = peek_alnum(self);
    if(string_in(keyword, g_control_keywords))
    {
        if(string_equal(keyword.begin, g_ret))
        {
            auto operator = string_init(self, g_ret);
            auto value = read_ret_statement(self);
            assert_types_match(self, value.type, ret_value.type, operator);
            assert_stars_match(self, value.type, ret_value.type, operator);
            return true;
        }
        else
        if(string_equal(keyword.begin, g_continue))
        {
            read_continue_statement(self);
            return true;
        }
        else
        if(string_equal(keyword.begin, g_break))
        {
            read_break_statement(self);
            return true;
        }
        else
        if(string_equal(keyword.begin, g_if))
        {
            read_if_else_statement(self, ret_value);
        }
        else
        if(string_equal(keyword.begin, g_else))
        {
            quit(self, "missing binding '%s'", g_if);
        }
        else
        if(string_equal(keyword.begin, g_while))
        {
            read_while_statement(self, ret_value);
        }
    }
    else
    if(string_in(keyword, g_type_keywords))
    {
        auto value = read_value_decl(self);
        value_list_append(self, &self->values, value);
        emit(self, g_opcode_alloca, value.slot, to_llvm_type(self, value.type).begin);
        match(self, g_semicolon);
    }
    else
    {
        read_space(self);
        if(peek_char(self) == *g_left_curl)
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

static bool
read_block(file_t* self, value_t ret_value)
{
    auto values = self->values.size;
    bool terminated = false;
    self->tabs += 1;
    match(self, g_left_curl);
    while(true)
    {
        read_space(self);
        if(peek_char(self) == *g_rite_curl)
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

static void
read_function(file_t* self)
{
    self->slot = 0;
    auto ret_value = read_value_decl(self);
    emit(self, g_opcode_define, to_llvm_type(self, ret_value.type).begin, ret_value.name.begin);
    auto values = read_function_decl_arg_list(self, &ret_value);
    value_list_append(self, &self->values, ret_value);
    for(size_t i = 0; i < values.size; i++)
    {
        value_list_append(self, &self->values, values.begin[i]);
    }
    emit(self, g_left_curl);
    emit(self, g_entry);
    bool terminated = read_block(self, ret_value);
    if(!terminated)
    {
        quit(self, "block missing 'ret' stastement");
    }
    emit(self, g_rite_curl);
    self->values.size -= values.size;
}

static bool
is_end_of_file(file_t* self)
{
    return self->code.at == self->code.size;
}

static void
read_program(file_t* self)
{
    emit(self, g_opcode_target);
    while(true)
    {
        read_space(self);
        if(is_end_of_file(self))
        {
            break;
        }
        read_function(self);
    }
}

static value_t
to_rvalue(file_t* self, value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = get_slot(self);
        emit(self, g_opcode_load, slot, to_llvm_type(self, value.type).begin, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

static const char* const*
get_operators(file_t* self, precedence_t precedence)
{
    if(precedence >= g_precedence_count)
    {
        quit(self, "unknown precedence level");
    }
    return g_operators_by_precedence[precedence];
}

static bool
is_relational(file_t* self, string_t operator)
{
    return string_in(operator, get_operators(self, g_precedence_relational_0))
        || string_in(operator, get_operators(self, g_precedence_relational_1));
}

static value_t
operate(file_t* self, value_t left, value_t rite, string_t operator)
{
    assert_types_match(self, left.type, rite.type, operator);
    if(string_in(operator, get_operators(self, g_precedence_assignment)))
    {
        assert_stars_match(self, left.type, rite.type, operator);
        assert_lvalue(self, left, operator);
        rite = to_rvalue(self, rite);
        emit(self, g_opcode_store, to_llvm_type(self, rite.type).begin, rite.slot, left.slot);
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
            string_equal(operator.begin, g_multiply)              ? g_opcode_mul                   :
            string_equal(operator.begin, g_divide)                ? g_opcode_sdiv                  :
            string_equal(operator.begin, g_add)                   ? g_opcode_add                   :
            string_equal(operator.begin, g_subtract)              ? g_opcode_sub                   :
            string_equal(operator.begin, g_equal_to)              ? g_opcode_equal_to              :
            string_equal(operator.begin, g_not_equal_to)          ? g_opcode_not_equal_to          :
            string_equal(operator.begin, g_less_than)             ? g_opcode_less_than             :
            string_equal(operator.begin, g_less_than_equal_to)    ? g_opcode_less_than_equal_to    :
            string_equal(operator.begin, g_greater_than)          ? g_opcode_greater_than          :
            string_equal(operator.begin, g_greater_than_equal_to) ? g_opcode_greater_than_equal_to :
            string_equal(operator.begin, g_bitwise_and)           ? g_opcode_bitwise_and           :
            string_equal(operator.begin, g_bitwise_or)            ? g_opcode_bitwise_or            :
            string_equal(operator.begin, g_bitwise_xor)           ? g_opcode_bitwise_xor           :
            string_equal(operator.begin, g_shift_left)            ? g_opcode_shift_left            :
            string_equal(operator.begin, g_shift_rite)            ? g_opcode_shift_rite            :
            nullptr;
        if(format)
        {
            emit(self, format, out.slot, left.type.name.begin, left.slot, rite.slot);
            if(is_relational(self, operator))
            {
                out.type.name = string_init(self, g_i1);
            }
            return out;
        }
        quit(self, "unknown operator '%s'", operator.begin);
        return (value_t) {};
    }
}

static value_t
read_ltor(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(self, precedence);
    auto left = with(self);
    while(string_in(peek_operator(self), operators))
    {
        auto operator = read_operator(self);
        auto rite = with(self);
        left = operate(self, left, rite, operator);
    }
    return left;
}

static value_t
read_rtol(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = get_operators(self, precedence);
    auto left = with(self);
    if(string_in(peek_operator(self), operators))
    {
        auto operator = read_operator(self);
        auto rite = read_rtol(self, with, precedence);
        left = operate(self, left, rite, operator);
    }
    return to_rvalue(self, left);
}

static value_t
load_direct(file_t* self)
{
    value_t value = {
        .type.name = string_init(self, g_i32),
        .slot = get_slot(self),
    };
    auto name = read_digit(self);
    emit(self, g_opcode_load_immediate, self->slot, value.type.name.begin, name.begin);
    return value;
}

static size_t
push_arg(file_t* self, type_t expected)
{
    auto arg = read_expression(self);
    auto operator = string_init(self, g_function);
    assert_types_match(self, arg.type, expected, operator);
    assert_stars_match(self, arg.type, expected, operator);
    auto slot = get_slot(self);
    emit(self, g_opcode_alloca, slot, g_ptr);
    emit(self, g_opcode_store, to_llvm_type(self, arg.type).begin, arg.slot, slot);
    return slot;
}

static slot_list_t
read_function_call_arg_list(file_t* self, value_t* found)
{
    slot_list_t list = {};
    match(self, g_left_paren);
    while(true)
    {
        read_space(self);
        if(peek_char(self) == *g_rite_paren)
        {
            break;
        }
        auto expected = found->types.begin[list.size];
        auto slot = push_arg(self, expected);
        slot_list_append(self, &list, slot);
        read_space(self);
        if(peek_char(self) == *g_comma)
        {
            match(self, g_comma);
            read_space(self);
            if(peek_char(self) == *g_rite_paren)
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

static value_t
call_function(file_t* self, value_t* found)
{
    auto list = read_function_call_arg_list(self, found);
    value_t value = {
        .slot = get_slot(self),
        .type = found->type,
    };
    emit(self, g_opcode_function_call, value.slot, to_llvm_type(self, found->type).begin, found->name.begin);
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

static value_t
load_indirect(file_t* self, value_t* found)
{
    value_t value = {
        .is_lvalue = true,
        .slot = get_slot(self),
        .type = found->type,
    };
    emit(self, g_opcode_gep, value.slot, found->slot, g_i32, 0);
    return value;
}

static value_t
get_address_of(file_t* self, value_t value)
{
    assert_lvalue(self, value, string_init(self, g_ampersand));
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

static value_t
dereference(file_t* self, value_t value)
{
    assert_lvalue(self, value, string_init(self, g_multiply));
    auto slot = get_slot(self);
    emit(self, g_opcode_load, slot, to_llvm_type(self, value.type).begin, value.slot);
    value.slot = slot;
    value.type.stars -= 1;
    return value;
}

static value_t
to_positive(file_t* self, value_t value)
{
    value = to_rvalue(self, value);
    type_t expect = {
        .name = string_init(self, g_i32)
    };
    assert_scalar(self, value.type, expect, string_init(self, g_add));
    return value;
}

static value_t
to_negative(file_t* self, value_t value)
{
    value = to_rvalue(self, value);
    type_t expect = {
        .name = string_init(self, g_i32)
    };
    auto operator = string_init(self, g_subtract);
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    emit(self, g_opcode_negative, out.slot, to_llvm_type(self, value.type).begin, value.slot);
    return out;
}

static value_t
to_bitwise_not(file_t* self, value_t value)
{
    value = to_rvalue(self, value);
    type_t expect = {
        .name = string_init(self, g_i32)
    };
    auto operator = string_init(self, g_bitwise_not);
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    emit(self, g_opcode_bitwise_not, out.slot, to_llvm_type(self, value.type).begin, value.slot);
    return out;
}

static value_t
to_not(file_t* self, value_t value)
{
    value = to_rvalue(self, value);
    type_t expect = {
        .name = string_init(self, g_i1)
    };
    auto operator = string_init(self, g_not);
    assert_scalar(self, value.type, expect, operator);
    value_t out = {
        .slot = get_slot(self),
        .type = value.type,
    };
    emit(self, g_opcode_not, out.slot, to_llvm_type(self, value.type).begin, value.slot);
    return out;
}

static value_t
read_p0(file_t*);

static value_t
read_unary(file_t* self)
{
    auto peek = peek_char(self);
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

static value_t
read_p0(file_t* self)
{
    read_space(self);
    auto peek = peek_char(self);
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
        read_space(self);
        if(peek_char(self) == *g_left_paren)
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

static value_t read_p1(file_t* self) { return read_ltor(self, read_p0, g_precedence_arithmetic_0); }
static value_t read_p2(file_t* self) { return read_ltor(self, read_p1, g_precedence_arithmetic_1); }
static value_t read_p3(file_t* self) { return read_ltor(self, read_p2, g_precedence_shift);        }
static value_t read_p4(file_t* self) { return read_ltor(self, read_p3, g_precedence_relational_0); }
static value_t read_p5(file_t* self) { return read_ltor(self, read_p4, g_precedence_relational_1); }
static value_t read_p6(file_t* self) { return read_ltor(self, read_p5, g_precedence_bitwise_and);  }
static value_t read_p7(file_t* self) { return read_ltor(self, read_p6, g_precedence_bitwise_xor);  }
static value_t read_p8(file_t* self) { return read_ltor(self, read_p7, g_precedence_bitwise_or);   }
static value_t read_p9(file_t* self) { return read_rtol(self, read_p8, g_precedence_assignment);   }

static value_t
read_expression(file_t* self)
{
    return read_p9(self);
}

int
main()
{
    auto file = file_init("test.n");
    read_program(&file);
}
