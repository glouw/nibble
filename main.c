#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static constexpr size_t g_string_size = 64;
static constexpr size_t g_list_size = 32;
static constexpr size_t g_code_size = 65536;
static constexpr size_t g_operators_per_precedence = 8;

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
    type_t type;
    string_t name;
    bool is_lvalue;
    size_t slot;
}
value_t;

typedef struct
{
    value_t begin[g_list_size];
    size_t size;
}
list_t;

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
    list_t values;
    size_t line;
    size_t slot;
    size_t tabs;
    size_t label;
}
file_t;

static constexpr chars_t g_left_curl = "{";
static constexpr chars_t g_rite_curl = "}";
static constexpr chars_t g_left_paren = "(";
static constexpr chars_t g_rite_paren = ")";
static constexpr chars_t g_semicolon = ";";
static constexpr chars_t g_newline = "\n";
static constexpr chars_t g_space = " ";
static constexpr chars_t g_tab = "\t";
static constexpr chars_t g_lower_begin = "a";
static constexpr chars_t g_lower_end = "z";
static constexpr chars_t g_upper_begin = "A";
static constexpr chars_t g_upper_end = "Z";
static constexpr chars_t g_digit_begin = "0";
static constexpr chars_t g_digit_end = "9";
static constexpr chars_t g_plus = "+";
static constexpr chars_t g_minus = "-";
static constexpr chars_t g_forward_slash = "/";
static constexpr chars_t g_asterisk = "*";
static constexpr chars_t g_equals = "=";
static constexpr chars_t g_equal_to = "==";
static constexpr chars_t g_i32 = "i32";
static constexpr chars_t g_ret = "ret";
static constexpr chars_t g_ptr = "ptr";
static constexpr chars_t g_if = "if";
static constexpr chars_t g_else = "else";
static constexpr chars_t g_ampersand = "&";
static constexpr chars_t g_label = "L%lu:";
static constexpr chars_t g_branch_if_else = "br i1 %%%llu, label %%L%lu, label %%L%lu";
static constexpr chars_t g_branch = "br label %%L%lu";
static constexpr chars_t g_opcode_mul = "%%%lu = mul %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_sdiv = "%%%lu = sdiv %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_add = "%%%lu = add %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_sub = "%%%lu = sub %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_equal_to = "%%%lu = icmp eq %s %%%lu, %%%lu";
static constexpr chars_t g_opcode_alloca = "%%%lu = alloca %s";
static constexpr chars_t g_opcode_target = "target triple = \"x86_64-pc-linux-gnu\"";
static constexpr chars_t g_opcode_gep = "%%%lu = getelementptr ptr, ptr %%%lu, %s %lu";
static constexpr chars_t g_opcode_load_immediate = "%%%lu = add %s %s, 0";
static constexpr chars_t g_opcode_ret = "ret %s %%%d";
static constexpr chars_t g_opcode_define = "define %s @%s()";
static constexpr chars_t g_opcode_load = "%%%lu = load %s, ptr %%%lu";
static constexpr chars_t g_opcode_store = "store %s %%%lu, ptr %%%lu";

static const char* const g_type_keywords[] = {
    g_i32, nullptr
};

static const char* const g_control_keywords[] = {
    g_ret, g_if, g_else, nullptr
};

static const char* const g_operator_chars[] = {
    g_ampersand, g_asterisk, g_forward_slash, g_plus, g_minus, g_equals, nullptr
};

static const char* const g_operators_by_precedence[g_precedence_count][g_operators_per_precedence + 1] = {
    [ g_precedence_arithmetic_0 ]  = { g_asterisk, g_forward_slash },
    [ g_precedence_arithmetic_1 ]  = { g_plus, g_minus             },
    [ g_precedence_shift        ]  = {                             },
    [ g_precedence_relational_0 ]  = {                             },
    [ g_precedence_relational_1 ]  = { g_equal_to                  },
    [ g_precedence_bitwise_and  ]  = {                             },
    [ g_precedence_bitwise_xor  ]  = {                             },
    [ g_precedence_bitwise_or   ]  = {                             },
    [ g_precedence_assignment   ]  = { g_equals                    },
};

static bool
value_is_pointer(value_t self)
{
    return self.type.stars > 0;
}

static void
file_quit(file_t* self, const char* format, ...)
{
    auto out = stderr;
    va_list args = {};
    va_start(args, format);
    fprintf(out, "error: line %lu: ", self->line);
    vfprintf(out, format, args);
    fprintf(out, g_newline);
    va_end(args);
    exit(1);
}

static void
file_emit(file_t* self, const char* format, ...)
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
file_string_append(file_t* self, string_t* string, char c)
{
    if(string->size == g_string_size - 1)
    {
        file_quit(self, "string '%s' truncated", string->begin);
    }
    string->begin[string->size++] = c;
}

static string_t
file_string_init(file_t* self, const chars_t chars)
{
    string_t string = {};
    while(*chars)
    {
        file_string_append(self, &string, *chars);
        chars += 1;
    }
    return string;
}

static string_t
file_value_to_type_name(file_t* self, value_t value)
{
    if(value_is_pointer(value))
    {
        return file_string_init(self, g_ptr);
    }
    else
    {
        return value.type.name;
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
value_in(string_t self, list_t* list)
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
file_list_append(file_t* self, list_t* list, value_t value)
{
    if(list->size == g_list_size)
    {
        file_quit(self, "list overflow");
    }
    list->begin[list->size++] = value;
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

static file_t
file_init(const char* path)
{
    file_t self = {};
    self.code = code_init(path);
    self.line = 1;
    return self;
}

static char
file_peek(file_t* self)
{
    return self->code.begin[self->code.at];
}

static void
file_step(file_t* self)
{
    self->code.at += 1;
    if(self->code.at == g_code_size - 1)
    {
        file_quit(self, "unexpected end of file");
    }
}

static bool
is_digit_char(char c)
{
    return c >= *g_digit_begin && c <= *g_digit_end;
}

static bool
is_lower_char(char c)
{
    return c >= *g_lower_begin && c <= *g_lower_end;
}

static bool
is_upper_char(char c)
{
    return c >= *g_upper_begin && c <= *g_upper_end;
}

static bool
is_alpha_char(char c)
{
    return is_lower_char(c) || is_upper_char(c);
}

static bool
is_alnum_char(char c)
{
    return is_alpha_char(c) || is_digit_char(c);
}

static bool
is_space_char(char c)
{
    return c == *g_space || c == *g_newline || c == *g_tab;
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

static void
file_read_space(file_t* self)
{
    while(true)
    {
        auto c = file_peek(self);
        if(is_space_char(c))
        {
            if(c == *g_newline)
            {
                self->line += 1;
            }
            file_step(self);
        }
        else
        {
            break;
        }
    }
}

static void
file_match(file_t* self, const char* expected)
{
    file_read_space(self);
    auto size = strlen(expected);
    string_t got = {};
    for(size_t i = 0; i < size; i++)
    {
        file_string_append(self, &got, file_peek(self));
        file_step(self);
    }
    if(!string_equal(got.begin, expected))
    {
        file_quit(self, "expected %s, got %s", expected, got.begin);
    }
}

static string_t
file_read(file_t* self, bool matches(char))
{
    file_read_space(self);
    string_t string = {};
    while(true)
    {
        auto c = file_peek(self);
        if(matches(c))
        {
            file_string_append(self, &string, c);
            file_step(self);
        }
        else
        {
            break;
        }
    }
    return string;
}

static string_t
file_read_operator(file_t* self)
{
    return file_read(self, is_operator_char);
}

static string_t
file_read_alnum(file_t* self)
{
    return file_read(self, is_alnum_char);
}

static string_t
file_read_digit(file_t* self)
{
    return file_read(self, is_digit_char);
}

static void
code_rewind(code_t* self, size_t by)
{
    self->at -= by;
}

static string_t
file_peek_operator(file_t* self)
{
    auto operator = file_read_operator(self);
    code_rewind(&self->code, operator.size);
    return operator;
}

static string_t
file_peek_alnum(file_t* self)
{
    auto alnum = file_read_alnum(self);
    code_rewind(&self->code, alnum.size);
    return alnum;
}

static size_t
file_read_stars(file_t* self)
{
    size_t stars = 0;
    while(true)
    {
        file_read_space(self);
        if(file_peek(self) == *g_asterisk)
        {
            stars += 1;
            file_step(self);
        }
        else
        {
            break;
        }
    }
    return stars;
}

static type_t
file_read_type(file_t* self)
{
    type_t type = {};
    type.name = file_read_alnum(self);
    type.stars = file_read_stars(self);
    return type;
}

static size_t
file_get_slot(file_t* self)
{
    self->slot += 1;
    return self->slot;
}

static size_t
file_get_label(file_t* self)
{
    self->label += 1;
    return self->label;
}

static value_t
file_read_value_declaration(file_t* self, bool is_function_declaration)
{
    value_t value = {};
    value.type = file_read_type(self);
    value.name = file_read_alnum(self);
    value.slot = file_get_slot(self);
    if(is_function_declaration)
    {
        /* for function pointers one day */
    }
    else
    {
        file_emit(self, g_opcode_alloca, value.slot, file_value_to_type_name(self, value).begin);
    }
    if(value_in(value.name, &self->values))
    {
        file_quit(self, "'%s' already declared", value.name.begin);
    }
    file_list_append(self, &self->values, value);
    return value;
}

static value_t
file_read_expression(file_t*);

static value_t
file_read_ret_statement(file_t* self)
{
    file_read_alnum(self);
    auto value = file_read_expression(self);
    file_match(self, g_semicolon);
    file_emit(self, g_opcode_ret, file_value_to_type_name(self, value).begin, value.slot);
    return value;
}

static bool
file_read_block(file_t* self);

static bool
file_read_statement(file_t* self);

static void
file_read_if_else_statement(file_t* self)
{
    auto l0 = file_get_label(self);
    auto l1 = file_get_label(self);
    auto l2 = file_get_label(self);
    file_read_alnum(self);
    file_match(self, g_left_paren);
    auto value = file_read_expression(self);
    file_match(self, g_rite_paren);
    file_emit(self, g_branch_if_else, value.slot, l0, l1);
    file_emit(self, g_label, l0 );
    auto terminated = file_read_statement(self);
    if(!terminated)
    {
        file_emit(self, g_branch, l2);
    }
    file_emit(self, g_label, l1);
    auto keyword = file_peek_alnum(self);
    if(string_equal(keyword.begin, g_else))
    {
        file_read_alnum(self);
        auto terminated = file_read_statement(self);
        if(!terminated)
        {
            file_emit(self, g_branch, l2);
        }
        file_emit(self, g_label, l2);
    }
}

static bool
file_read_statement(file_t* self)
{
    bool terminated = false;
    auto keyword = file_peek_alnum(self);
    if(string_in(keyword, g_control_keywords))
    {
        if(string_equal(keyword.begin, g_ret))
        {
            file_read_ret_statement(self);
            terminated = true;
        }
        else
        if(string_equal(keyword.begin, g_if))
        {
            file_read_if_else_statement(self);
        }
    }
    else
    if(string_in(keyword, g_type_keywords))
    {
        file_read_value_declaration(self, false);
        file_match(self, g_semicolon);
    }
    else
    {
        file_read_space(self);
        if(file_peek(self) == *g_left_curl)
        {
            terminated = file_read_block(self);
        }
        else
        {
            file_read_expression(self);
            file_match(self, g_semicolon);
        }
    }
    return terminated;
}

static bool
file_read_block(file_t* self)
{
    auto values = self->values.size;
    bool terminated = false;
    self->tabs += 1;
    file_match(self, g_left_curl);
    while(true)
    {
        file_read_space(self);
        if(file_peek(self) == *g_rite_curl)
        {
            break;
        }
        if(terminated)
        {
            file_quit(self, "block was terminated");
        }
        terminated = file_read_statement(self);
    }
    file_match(self, g_rite_curl);
    self->tabs -= 1;
    self->values.size = values;
    return terminated;
}

static void
file_read_function(file_t* self)
{
    auto value = file_read_value_declaration(self, true);
    file_match(self, g_left_paren);
    file_match(self, g_rite_paren);
    file_emit(self, g_opcode_define, file_value_to_type_name(self, value).begin, value.name.begin);
    file_emit(self, g_left_curl);
    file_read_block(self);
    file_emit(self, g_rite_curl);
}

static bool
file_is_at_end(file_t* self)
{
    return self->code.at == self->code.size;
}

static void
file_read_program(file_t* self)
{
    file_emit(self, g_opcode_target);
    while(true)
    {
        file_read_space(self);
        if(file_is_at_end(self))
        {
            break;
        }
        file_read_function(self);
    }
}

static void
file_value_types_must_match(file_t* self, value_t left, value_t rite, string_t operator)
{
    if(!string_equal(left.type.name.begin, rite.type.name.begin))
    {
        file_quit(self, "type mismatch with '%s'", operator.begin);
    }
}

static void
file_value_pointer_levels_must_match(file_t* self, value_t left, value_t rite, string_t operator)
{
    if(left.type.stars != rite.type.stars)
    {
        file_quit(self, "pointer level mismatch with '%s'", operator.begin);
    }
}

static void
file_values_must_be_scalar(file_t* self, value_t left, value_t rite, string_t operator)
{
    if(value_is_pointer(left) || value_is_pointer(rite))
    {
        file_quit(self, "expected scalars with '%s'", operator.begin);
    }
}

static void
file_assert_lvalue(file_t* self, value_t left, string_t operator)
{
    if(!left.is_lvalue)
    {
        file_quit(self, "expected lvalue with '%s'", operator.begin);
    }
}

static value_t
file_value_to_rvalue(file_t* self, value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = file_get_slot(self);
        file_emit(self, g_opcode_load, slot, file_value_to_type_name(self, value).begin, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

static value_t
file_operate_assignment(file_t* self, value_t left, value_t rite, string_t operator)
{
    file_value_pointer_levels_must_match(self, left, rite, operator);
    file_assert_lvalue(self, left, operator);
    rite = file_value_to_rvalue(self, rite);
    file_emit(self, g_opcode_store, file_value_to_type_name(self, rite).begin, rite.slot, left.slot, left.slot);
    return file_value_to_rvalue(self, left);
}

static value_t
file_operate_binary(file_t* self, value_t left, value_t rite, string_t operator)
{
    file_values_must_be_scalar(self, left, rite, operator);
    left = file_value_to_rvalue(self, left);
    rite = file_value_to_rvalue(self, rite);
    value_t out = {};
    out.slot = file_get_slot(self);
    out.type = left.type;
    if(string_equal(operator.begin, g_asterisk))
    {
        file_emit(self, g_opcode_mul, out.slot, left.type.name.begin, left.slot, rite.slot);
    }
    else
    if(string_equal(operator.begin, g_forward_slash))
    {
        file_emit(self, g_opcode_sdiv, out.slot, left.type.name.begin, left.slot, rite.slot);
    }
    else
    if(string_equal(operator.begin, g_plus))
    {
        file_emit(self, g_opcode_add, out.slot, left.type.name.begin, left.slot, rite.slot);
    }
    else
    if(string_equal(operator.begin, g_minus))
    {
        file_emit(self, g_opcode_sub, out.slot, left.type.name.begin, left.slot, rite.slot);
    }
    else
    if(string_equal(operator.begin, g_equal_to))
    {
        file_emit(self, g_opcode_equal_to, out.slot, left.type.name.begin, left.slot, rite.slot);
    }
    else
    {
        file_quit(self, "unknown operator '%s'", operator.begin);
    }
    return out;
}

static value_t
file_operate(file_t* self, value_t left, value_t rite, string_t operator)
{
    file_value_types_must_match(self, left, rite, operator);
    return (string_equal(operator.begin, g_equals) ? file_operate_assignment : file_operate_binary)(self, left, rite, operator);
}

static const char* const*
file_get_operators_by_precedence(file_t* self, precedence_t precedence)
{
    if(precedence >= g_precedence_count)
    {
        file_quit(self, "unknown precedence level");
    }
    return g_operators_by_precedence[precedence];
}

static value_t
file_read_expression_left_to_rite(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = file_get_operators_by_precedence(self, precedence);
    auto left = with(self);
    while(string_in(file_peek_operator(self), operators))
    {
        auto operator = file_read_operator(self);
        auto rite = with(self);
        left = file_operate(self, left, rite, operator);
    }
    return left;
}

static value_t
file_read_expression_rite_to_left(file_t* self, value_t with(file_t*), precedence_t precedence)
{
    auto operators = file_get_operators_by_precedence(self, precedence);
    auto left = with(self);
    if(string_in(file_peek_operator(self), operators))
    {
        auto operator = file_read_operator(self);
        auto rite = file_read_expression_rite_to_left(self, with, precedence);
        left = file_operate(self, left, rite, operator);
    }
    return file_value_to_rvalue(self, left);
}

static value_t
file_direct_load(file_t* self)
{
    value_t value = {};
    value.type.name = file_string_init(self, g_i32);
    value.slot = file_get_slot(self);
    auto name = file_read_digit(self);
    file_emit(self, g_opcode_load_immediate, self->slot, value.type.name.begin, name.begin);
    return value;
}

static value_t
file_indirect_load(file_t* self)
{
    auto name = file_read_alnum(self);
    auto found = value_in(name, &self->values);
    if(!found)
    {
        file_quit(self, "'%s' not declared", name.begin);
    }
    value_t value = {};
    value.is_lvalue = true;
    value.slot = file_get_slot(self);
    value.type = found->type;
    file_emit(self, g_opcode_gep, value.slot, found->slot, g_i32, 0);
    return value;
}

static value_t
file_get_address_of_value(file_t* self, value_t value)
{
    file_assert_lvalue(self, value, file_string_init(self, g_ampersand));
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

static value_t
file_dereference_value(file_t* self, value_t value)
{
    file_assert_lvalue(self, value, file_string_init(self, g_asterisk));
    auto slot = file_get_slot(self);
    file_emit(self, g_opcode_load, slot, file_value_to_type_name(self, value).begin, value.slot);
    value.slot = slot;
    value.type.stars -= 1;
    return value; // Stays lvalue
}

static value_t
file_p0(file_t* self);

static value_t
file_read_unary_expression(file_t* self)
{
    auto peek = file_peek(self);
    if(peek == *g_ampersand)
    {
        file_match(self, g_ampersand);
        auto value = file_p0(self);
        return file_get_address_of_value(self, value);
    }
    if(peek == *g_asterisk)
    {
        file_match(self, g_asterisk);
        auto value = file_p0(self);
        return file_dereference_value(self, value);
    }
    file_quit(self, "compiler error: '%s'", __func__);
    return (value_t) {};
}

static value_t
file_p0(file_t* self)
{
    file_read_space(self);
    auto peek = file_peek(self);
    if(is_digit_char(peek))
    {
        return file_direct_load(self);
    }
    if(is_alpha_char(peek))
    {
        return file_indirect_load(self);
    }
    if(peek == *g_left_paren)
    {
        file_match(self, g_left_paren);
        auto value = file_read_expression(self);
        file_match(self, g_rite_paren);
        return value;
    }
    return file_read_unary_expression(self);
}

static value_t file_p1(file_t* self) { return file_read_expression_left_to_rite(self, file_p0, g_precedence_arithmetic_0); }
static value_t file_p2(file_t* self) { return file_read_expression_left_to_rite(self, file_p1, g_precedence_arithmetic_1); }
static value_t file_p3(file_t* self) { return file_read_expression_left_to_rite(self, file_p2, g_precedence_shift);        }
static value_t file_p4(file_t* self) { return file_read_expression_left_to_rite(self, file_p3, g_precedence_relational_0); }
static value_t file_p5(file_t* self) { return file_read_expression_left_to_rite(self, file_p4, g_precedence_relational_1); }
static value_t file_p6(file_t* self) { return file_read_expression_left_to_rite(self, file_p5, g_precedence_bitwise_and);  }
static value_t file_p7(file_t* self) { return file_read_expression_left_to_rite(self, file_p6, g_precedence_bitwise_xor);  }
static value_t file_p8(file_t* self) { return file_read_expression_left_to_rite(self, file_p7, g_precedence_bitwise_or);   }
static value_t file_p9(file_t* self) { return file_read_expression_rite_to_left(self, file_p8, g_precedence_assignment);   }

static value_t
file_read_expression(file_t* self)
{
    return file_p9(self);
}

int
main()
{
    auto file = file_init("test.n");
    file_read_program(&file);
}
