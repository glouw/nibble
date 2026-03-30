#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

constexpr size_t g_string_size = 32;
constexpr size_t g_list_size = 32;
constexpr size_t g_code_size = 65536;
constexpr size_t g_precedence_levels = 16;

typedef char chars_t[g_string_size];

constexpr chars_t g_left_curl = "{";
constexpr chars_t g_rite_curl = "}";
constexpr chars_t g_left_paren = "(";
constexpr chars_t g_rite_paren = ")";
constexpr chars_t g_semicolon = ";";
constexpr chars_t g_newline = "\n";
constexpr chars_t g_space = " ";
constexpr chars_t g_tab = "\t";
constexpr chars_t g_lower_begin = "a";
constexpr chars_t g_lower_end = "z";
constexpr chars_t g_upper_begin = "A";
constexpr chars_t g_upper_end = "Z";
constexpr chars_t g_digit_begin = "0";
constexpr chars_t g_digit_end = "9";
constexpr chars_t g_plus = "+";
constexpr chars_t g_minus = "-";
constexpr chars_t g_forward_slash = "/";
constexpr chars_t g_asterisk = "*";
constexpr chars_t g_equals = "=";
constexpr chars_t g_i32 = "i32";
constexpr chars_t g_ret = "ret";
constexpr chars_t g_ptr = "ptr";
constexpr chars_t g_ampersand = "&";

const char* g_type_keywords[] = {
    g_i32,
    nullptr
};

const char* g_control_keywords[] = {
    g_ret,
    nullptr
};

const char* g_operator_chars[] = {
    g_ampersand,
    g_asterisk,
    g_forward_slash,
    g_plus,
    g_minus,
    g_equals,
    nullptr
};

const char* g_operators_by_precedence[g_precedence_levels][4] = {
    [0] = {
        nullptr
    },
    [1] = {
        g_ampersand,
        g_asterisk,
        nullptr
    },
    [2] = {
        g_asterisk,
        g_forward_slash,
        nullptr
    },
    [3] = {
        g_plus,
        g_minus,
        nullptr
    },
    [4] = {
        nullptr
    },
    [5] = {
        nullptr
    },
    [6] = {
        g_equals,
        nullptr
    },
    [7] = {
        nullptr
    },
    [8] = {
        nullptr
    },
    [9] = {
        nullptr
    },
    [10] = {
        nullptr
    },
    [11] = {
        nullptr
    },
    [12] = {
        nullptr
    },
    [13] = {
        nullptr
    },
    [14] = {
        nullptr
    },
};

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
}
file_t;

void
file_quit(file_t* self, const char* format, ...)
{
    va_list args = {};
    va_start(args, format);
    fprintf(stderr, "error: line %lu: ", self->line);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

void
file_emit(file_t* self, const char* format, ...)
{
    va_list args = {};
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void
file_string_append(file_t* self, string_t* string, char c)
{
    if(string->size == g_string_size - 1)
    {
        file_quit(self, "string %s truncated\n", string->begin);
    }
    string->begin[string->size++] = c;
}

string_t
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

string_t
file_value_to_type_name(file_t* self, value_t value)
{
    if(value.type.stars > 0)
    {
        return file_string_init(self, g_ptr);
    }
    else
    {
        return value.type.name;
    }
}

bool
string_equal(const char* self, const char* other)
{
    return strcmp(self, other) == 0;
}

bool
string_in(string_t self, const char** array)
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

value_t*
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

void
file_list_append(file_t* self, list_t* list, value_t value)
{
    if(list->size == g_list_size)
    {
        file_quit(self, "list overflow\n");
    }
    list->begin[list->size++] = value;
}

code_t
code_init(char* path)
{
    code_t self = {};
    auto fp = fopen(path, "r");
    self.size = fread(self.begin, sizeof(char), g_code_size - 1, fp),
    fclose(fp);
    return self;
}

file_t
file_init(char* path)
{
    file_t self = {};
    self.code = code_init(path);
    self.line = 1;
    return self;
}

char
file_peek(file_t* self)
{
    return self->code.begin[self->code.at];
}

void
file_step(file_t* self)
{
    self->code.at += 1;
    if(self->code.at == g_code_size - 1)
    {
        file_quit(self, "unexpected end of file\n");
    }
}

bool
is_digit_char(char c)
{
    return c >= *g_digit_begin && c <= *g_digit_end;
}

bool
is_lower_char(char c)
{
    return c >= *g_lower_begin && c <= *g_lower_end;
}

bool
is_upper_char(char c)
{
    return c >= *g_upper_begin && c <= *g_upper_end;
}

bool
is_alpha_char(char c)
{
    return is_lower_char(c) || is_upper_char(c);
}

bool
is_alnum_char(char c)
{
    return is_alpha_char(c) || is_digit_char(c);
}

bool
is_space_char(char c)
{
    return c == *g_space || c == *g_newline || c == *g_tab;
}

bool
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

void
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

void
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
        file_quit(self, "expected %s, got %s\n", expected, got.begin);
    }
}

string_t
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

string_t
file_read_operator(file_t* self)
{
    return file_read(self, is_operator_char);
}

string_t
file_read_alnum(file_t* self)
{
    return file_read(self, is_alnum_char);
}

string_t
file_read_digit(file_t* self)
{
    return file_read(self, is_digit_char);
}

void
file_rewind(file_t* self, size_t by)
{
    self->code.at -= by;
}

string_t
file_peek_operator(file_t* self)
{
    auto operator = file_read_operator(self);
    file_rewind(self, operator.size);
    return operator;
}

string_t
file_peek_alnum(file_t* self)
{
    auto alnum = file_read_alnum(self);
    file_rewind(self, alnum.size);
    return alnum;
}

size_t
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

type_t
file_read_type(file_t* self)
{
    type_t type = {};
    type.name = file_read_alnum(self);
    type.stars = file_read_stars(self);
    return type;
}

size_t
file_get_slot(file_t* self)
{
    self->slot += 1;
    return self->slot;
}

value_t
file_read_value_decl(file_t* self, bool is_funtion_decl)
{
    value_t value = {};
    value.type = file_read_type(self);
    value.name = file_read_alnum(self);
    value.slot = file_get_slot(self);
    if(is_funtion_decl)
    {
        /* for function pointers one day */
    }
    else
    {
        file_emit(self, "\t%%%lu = alloca %s\n", value.slot, file_value_to_type_name(self, value).begin);
    }
    value.is_lvalue = true;
    if(value_in(value.name, &self->values))
    {
        file_quit(self, "'%s' already declared\n", value.name.begin);
    }
    file_list_append(self, &self->values, value);
    return value;
}

value_t file_read_expression(file_t*);

void
file_read_statement(file_t* self)
{
    auto keyword = file_peek_alnum(self);
    if(string_in(keyword, g_control_keywords))
    {
        keyword = file_read_alnum(self);
        auto value = file_read_expression(self);
        if(string_equal(keyword.begin, g_ret))
        {
            file_emit(self, "\tret %s %%%d\n", file_value_to_type_name(self, value).begin, value.slot);
        }
        file_match(self, g_semicolon);
    }
    else
    if(string_in(keyword, g_type_keywords))
    {
        file_read_value_decl(self, false);
        file_match(self, g_semicolon);
    }
    else
    {
        file_read_expression(self);
        file_match(self, g_semicolon);
    }
}

void
file_read_block(file_t* self)
{
    file_match(self, g_left_curl);
    while(true)
    {
        file_read_space(self);
        if(file_peek(self) == *g_rite_curl)
        {
            break;
        }
        file_read_statement(self);
    }
    file_match(self, g_rite_curl);
}

void
file_read_function(file_t* self)
{
    auto value = file_read_value_decl(self, true);
    file_match(self, g_left_paren);
    file_match(self, g_rite_paren);
    file_emit(self, "define %s @%s()\n", file_value_to_type_name(self, value).begin, value.name.begin);
    file_emit(self, "{\n");
    file_read_block(self);
    file_emit(self, "}\n");
}

bool
file_at_end(file_t* self)
{
    return self->code.at == self->code.size;
}

void
file_read_program(file_t* self)
{
    file_emit(self, "target triple = \"x86_64-pc-linux-gnu\"\n");
    while(true)
    {
        file_read_space(self);
        if(file_at_end(self))
        {
            break;
        }
        file_read_function(self);
    }
}

void
file_value_types_must_match(file_t* self, value_t left, value_t rite, string_t operator)
{
    if(!string_equal(left.type.name.begin, rite.type.name.begin))
    {
        file_quit(self, "type mismatch with operator '%s'\n", operator.begin);
    }
}

void
file_value_stars_must_match(file_t* self, value_t left, value_t rite, string_t operator)
{
    if(left.type.stars != rite.type.stars)
    {
        file_quit(self, "type pointer level mismatch with operator '%s'\n", operator.begin);
    }
}

void
file_assert_lvalue(file_t* self, value_t left, string_t operator)
{
    if(!left.is_lvalue)
    {
        file_quit(self, "expected lvalue with operator '%s'\n", operator.begin);
    }
}

value_t
file_to_rvalue(file_t* self, value_t value)
{
    if(value.is_lvalue)
    {
        auto slot = file_get_slot(self);
        file_emit(self, "\t%%%lu = load %s, ptr %%%lu\n", slot, file_value_to_type_name(self, value).begin, value.slot);
        value.is_lvalue = false;
        value.slot = slot;
    }
    return value;
}

value_t
file_operate(file_t* self, value_t left, value_t rite, string_t operator)
{
    file_value_types_must_match(self, left, rite, operator);
    file_value_stars_must_match(self, left, rite, operator);
    if(string_equal(operator.begin, g_equals))
    {
        file_assert_lvalue(self, left, operator);
        file_emit(self, "\tstore %s %%%lu, ptr %%%lu\n", file_value_to_type_name(self, rite).begin, rite.slot, left.slot, left.slot);
    }
    else
    {
        const char* format = nullptr;
        auto slot = file_get_slot(self);
        if(string_equal(operator.begin, g_asterisk))
        {
            format = "\t%%%lu = mul %s %%%lu, %%%lu\n";
        }
        else
        if(string_equal(operator.begin, g_forward_slash))
        {
            format = "\t%%%lu = sdiv %s %%%lu, %%%lu\n";
        }
        else
        if(string_equal(operator.begin, g_plus))
        {
            format = "\t%%%lu = add %s %%%lu, %%%lu\n";
        }
        else
        if(string_equal(operator.begin, g_minus))
        {
            format = "\t%%%lu = sub %s %%%lu, %%%lu\n";
        }
        else
        {
            file_quit(self, "unknown operator '%s'\n", operator.begin);
        }
        file_emit(self, format, slot, left.type.name.begin, left.slot, rite.slot);
        left.slot = slot;
    }
    return left;
}

const char**
file_get_operators_by_precedence(file_t* self, size_t precedence)
{
    if(precedence >= g_precedence_levels)
    {
        file_quit(self, "unknown precedence level\n");
    }
    return g_operators_by_precedence[precedence];
}

value_t
file_read_expression_left_to_rite(file_t* self, value_t with(file_t*), size_t precedence)
{
    auto operators = file_get_operators_by_precedence(self, precedence);
    auto left = with(self);
    while(string_in(file_peek_operator(self), operators))
    {
        auto operator = file_read_operator(self);
        auto rite = with(self);
        left = file_to_rvalue(self, left);
        rite = file_to_rvalue(self, rite);
        left = file_operate(self, left, rite, operator);
    }
    return left;
}

value_t
file_read_expression_rite_to_left(file_t* self, value_t with(file_t*), size_t precedence)
{
    auto operators = file_get_operators_by_precedence(self, precedence);
    auto left = with(self);
    if(string_in(file_peek_operator(self), operators))
    {
        auto operator = file_read_operator(self);
        auto rite = file_read_expression_rite_to_left(self, with, precedence);
        rite = file_to_rvalue(self, rite);
        left = file_operate(self, left, rite, operator);
    }
    return file_to_rvalue(self, left);
}

value_t
file_direct_load(file_t* self)
{
    value_t value = {};
    value.type.name = file_string_init(self, g_i32);
    value.slot = file_get_slot(self);
    auto name = file_read_alnum(self);
    file_emit(self, "\t%%%lu = add %s %s, 0\n", self->slot, value.type.name.begin, name.begin);
    return value;
}

value_t
file_indirect_load(file_t* self)
{
    auto name = file_read_alnum(self);
    auto found = value_in(name, &self->values);
    if(!found)
    {
        file_quit(self, "'%s' not declared\n", name.begin);
    }
    value_t value = {};
    value.is_lvalue = true;
    value.slot = file_get_slot(self);
    value.type = found->type;
    file_emit(self, "\t%%%lu = getelementptr ptr, ptr %%%lu, i32 0\n", value.slot, found->slot);
    return value;
}

value_t
file_get_address_of_value(file_t* self, value_t value)
{
    value.type.stars += 1;
    value.is_lvalue = false;
    return value;
}

value_t
file_dereference_value(file_t* self, value_t value)
{
    auto slot = file_get_slot(self);
    file_emit(self, "\t%%%lu = load %s, ptr %%%lu\n", slot, file_value_to_type_name(self, value).begin, value.slot);
    value.slot = slot;
    value.type.stars -= 1;
    return value;
}

value_t
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
    if(peek == *g_ampersand)
    {
        file_match(self, g_ampersand);
        auto value = file_p0(self);
        file_assert_lvalue(self, value, file_string_init(self, g_ampersand));
        return file_get_address_of_value(self, value);
    }
    if(peek == *g_asterisk)
    {
        file_match(self, g_asterisk);
        auto value = file_p0(self);
        file_assert_lvalue(self, value, file_string_init(self, g_asterisk));
        return file_dereference_value(self, value);
    }
    file_quit(self, "compiler error: '%s'\n", __func__);
    return (value_t) {};
}

value_t file_p1(file_t* self) { return file_read_expression_left_to_rite(self, file_p0, 0); }
value_t file_p2(file_t* self) { return file_read_expression_left_to_rite(self, file_p1, 1); }
value_t file_p3(file_t* self) { return file_read_expression_left_to_rite(self, file_p2, 2); }
value_t file_p4(file_t* self) { return file_read_expression_left_to_rite(self, file_p3, 3); }
value_t file_p5(file_t* self) { return file_read_expression_left_to_rite(self, file_p4, 4); }
value_t file_p6(file_t* self) { return file_read_expression_left_to_rite(self, file_p5, 5); }
value_t file_p7(file_t* self) { return file_read_expression_rite_to_left(self, file_p6, 6); }

value_t
file_read_expression(file_t* self)
{
    return file_p7(self);
}

int main()
{
    auto file = file_init("test.e");
    file_read_program(&file);
}
