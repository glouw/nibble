#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr int g_file_size = 4096;
constexpr int g_string_size = 32;
constexpr int g_values_size = 128;

typedef struct
{
    char begin[g_string_size];
    int size;
}
string_t;

typedef struct
{
    string_t name;
    int stars;
}
type_t;

typedef struct
{
    type_t type;
    string_t name;
    bool is_lvalue;
    bool is_rvalue;
    int slot;
}
value_t;

value_t g_values[g_values_size] = {};
char g_text[g_file_size] = {};
int g_values_at = 0;
int g_text_at = 0;
int g_slot = 0;
int g_file_line = 1;

#define quit_d(...)                                   \
    fprintf(stderr, "error: line %d: ", g_file_line), \
    fprintf(stderr, __VA_ARGS__),                     \
    fprintf(stderr, "\n"),                            \
    exit(1)

#define emit_d(...)               \
    fprintf(stdout, __VA_ARGS__), \
    fprintf(stdout, "\n")

void str_push(string_t* self, char ch)
{
    if(self->size > g_string_size - 2)
    {
        quit_d("string `%s` exceeded string buffer size", self->begin);
    }
    self->begin[self->size] = ch;
    self->size += 1;
}

bool str_equal(string_t self, char* other)
{
    return strcmp(self.begin, other) == 0;
}

void reserve_slot()
{
    g_slot += 1;
}

char peek()
{
    return g_text[g_text_at];
}

void step()
{
    g_text_at += 1;
}

void step_back(int by)
{
    g_text_at -= by;
}

bool is_space_ch(char ch)
{
    return ch == ' '
        || ch == '\n'
        || ch == '\r';
}

bool is_operator_ch(char ch)
{
    return ch == '+'
        || ch == '-'
        || ch == '*'
        || ch == '/'
        || ch == '%'
        || ch == '='
        || ch == '<'
        || ch == '>'
        || ch == '&'
        || ch == '^'
        || ch == '|';
}

bool is_lower_ch(char ch)
{
    return ch >= 'a'
        && ch <= 'z';
}

bool is_upper_ch(char ch)
{
    return ch >= 'A'
        && ch <= 'Z';
}

bool is_number_ch(char ch)
{
    return ch >= '0'
        && ch <= '9';
}

bool is_string_ch(char ch)
{
    return is_lower_ch(ch)
        || is_upper_ch(ch)
        || is_number_ch(ch)
        || ch == '_';
}

bool is_operator_s(string_t operator, int pres)
{
    if(pres == 1)
    {
        return str_equal(operator, "*")
            || str_equal(operator, "/")
            || str_equal(operator, "%");
    }
    if(pres == 2)
    {
        return str_equal(operator, "+")
            || str_equal(operator, "-");
    }
    if(pres == 3)
    {
        return str_equal(operator, "<<")
            || str_equal(operator, ">>");
    }
    if(pres == 4)
    {
        return str_equal(operator, "&");
    }
    if(pres == 5)
    {
        return str_equal(operator, "^");
    }
    if(pres == 6)
    {
        return str_equal(operator, "|");
    }
    if(pres == 7)
    {
        return str_equal(operator, "=");
    }
    quit_d("compiler error: unknown operator precedence %d and/or operator `%s`", pres, operator.begin);
}

bool is_type(string_t type)
{
    // Will need runtime types from structs, etc.
    return str_equal(type, "i32");
}

bool is_keyword(string_t keyword)
{
    return str_equal(keyword, "ret") || is_type(keyword);
}

void spaces()
{
    while(is_space_ch(peek()))
    {
        if(peek() == '\n')
        {
            g_file_line += 1;
        }
        step();
    }
}

int is_done()
{
    spaces();
    return peek() == '\0';
}

void match(char ch)
{
    spaces();
    if(ch != peek())
    {
        quit_d("expected `%c` but got `%c`", ch, peek());
    }
    step();
}

string_t token(bool condition(char))
{
    spaces();
    string_t out = {};
    while(condition(peek()))
    {
        str_push(&out, peek());
        step();
    }
    return out;
}

string_t number()
{
    return token(is_number_ch);
}

string_t operator()
{
    return token(is_operator_ch);
}

string_t string()
{
    return token(is_string_ch);
}

string_t keyword()
{
    return string();
}

string_t peek_keyword()
{
    auto key = keyword();
    step_back(key.size);
    return key;
}

string_t peek_operator()
{
    auto oper = operator();
    step_back(oper.size);
    return oper;
}

value_t* lookup_value(string_t name)
{
    for(int i = 0; i < g_values_at; i++)
    {
        auto value = &g_values[i];
        if(str_equal(value->name, name.begin))
        {
            return value;
        }
    }
    return nullptr;
}

int stars()
{
    int count = 0;
    while(true)
    {
        spaces();
        if(peek() != '*')
        {
            break;
        }
        else
        {
            match('*');
            count += 1;
        }
    }
    return count;
}

type_t type()
{
    type_t type = {
        .name = keyword(),
        .stars = stars(),
    };
    return type;
}

value_t declare()
{
    value_t value = (value_t) {
        .type = type(),
        .name = string(),
        .is_lvalue = true,
    };
    if(lookup_value(value.name))
    {
        quit_d("`%s` already defined", value.name.begin);
    }
    g_values[g_values_at] = value;
    g_values_at += 1;
    return value;
}

string_t star_str(value_t self, int extra)
{
    string_t string = {};
    for(int i = 0; i < self.type.stars + extra; i++)
    {
        str_push(&string, '*');
    }
    return string;
}

void type_check(value_t left, value_t rite)
{
    if(left.type.stars != rite.type.stars)
    {
        quit_d(
            "variables `%s` and `%s` mismatch with pointer level %d and %d",
            left.name.begin,
            rite.name.begin,
            left.type.stars,
            rite.type.stars);
    }
    if(str_equal(left.type.name, rite.type.name.begin) == false)
    {
        quit_d(
            "variables `%s` and `%s` mismatch with types `%s` and `%s`",
            left.name.begin,
            rite.name.begin,
            left.type.name.begin,
            rite.type.name.begin);
    }
}

void target()
{
    emit_d("target triple = \"x86_64-pc-linux-gnu\"");
}

void load(value_t self)
{
    emit_d(
        "\t%%%d = load %s%s, %s%s %%%s",
        self.slot,
        self.type.name.begin,
        star_str(self, 0).begin,
        self.type.name.begin,
        star_str(self, 1).begin,
        self.name.begin);
}

void store(value_t left, value_t rite)
{
    type_check(left, rite);
    emit_d(
        "\tstore %s%s %%%d, %s%s %%%s",
        rite.type.name.begin,
        star_str(left, 0).begin,
        rite.slot,
        left.type.name.begin,
        star_str(left, 1).begin,
        left.name.begin);
}

void alloca(value_t self)
{
    emit_d("\t%%%s = alloca %s%s",
        self.name.begin,
        self.type.name.begin,
        star_str(self, 0).begin);
}

void ret(value_t self)
{
    emit_d("\tret %s%s %%%d",
        self.type.name.begin,
        star_str(self, 0).begin,
        self.slot);
}

void define(value_t self)
{
    emit_d(
        "define %s%s @%s()",
        self.type.name.begin,
        star_str(self, 0).begin,
        self.name.begin);
}

void push(value_t self)
{
    emit_d(
        "\t%%%d = add %s 0, %s",
        self.slot,
        self.type.name.begin,
        self.name.begin);
}

void multiply(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = mul %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void divide(value_t left, value_t rite)
{
    // Needs unsigned.
    emit_d(
        "\t%%%d = sdiv %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void modulo(value_t left, value_t rite)
{
    // Needs unsigned.
    emit_d(
        "\t%%%d = srem %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void add(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = add %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void subtract(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = sub %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void shift_left(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = shl %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void shift_rite(value_t left, value_t rite)
{
    // Needs unsigned.
    emit_d(
        "\t%%%d = ashr %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void and(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = and %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void xor(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = xor %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void or(value_t left, value_t rite)
{
    emit_d(
        "\t%%%d = or %s %%%d, %%%d",
        g_slot,
        left.type.name.begin,
        left.slot,
        rite.slot);
}

void assignment_op(value_t left, value_t rite, string_t oper)
{
    type_check(left, rite);
    if(str_equal(oper, "="))
    {
        store(left, rite);
    }
}

void binary_op(value_t left, value_t rite, string_t oper)
{
    type_check(left, rite);
    if(str_equal(oper, "*"))
    {
        multiply(left, rite);
    }
    else
    if(str_equal(oper, "/"))
    {
        divide(left, rite);
    }
    else
    if(str_equal(oper, "%"))
    {
        modulo(left, rite);
    }
    else
    if(str_equal(oper, "+"))
    {
        add(left, rite);
    }
    else
    if(str_equal(oper, "-"))
    {
        subtract(left, rite);
    }
    else
    if(str_equal(oper, "<<"))
    {
        shift_left(left, rite);
    }
    else
    if(str_equal(oper, ">>"))
    {
        shift_rite(left, rite);
    }
    else
    if(str_equal(oper, "&"))
    {
        and(left, rite);
    }
    else
    if(str_equal(oper, "^"))
    {
        xor(left, rite);
    }
    else
    if(str_equal(oper, "|"))
    {
        or(left, rite);
    }
    else
    {
        quit_d("compiler error: unknown operator `%s`", oper.begin);
    }
}

value_t to_rvalue(value_t self)
{
    if(self.is_lvalue)
    {
        reserve_slot();
        self.slot = g_slot;
        load(self);
        self.is_lvalue = false;
        self.is_rvalue = true;
    }
    return self;
}

void ensure_lvalue(value_t self)
{
    if(self.is_rvalue)
    {
        quit_d("`%s` must be lvalue", self.name.begin);
    }
}

value_t left_to_rite(value_t with(), int pres)
{
    auto left = with();
    while(is_operator_s(peek_operator(), pres))
    {
        auto oper = operator();
        auto rite = with();
        left = to_rvalue(left);
        rite = to_rvalue(rite);
        reserve_slot();
        binary_op(left, rite, oper);
        left.slot = g_slot;
    }
    return left;
}

value_t rite_to_left(value_t with(), int pres)
{
    auto left = with();
    if(is_operator_s(peek_operator(), pres))
    {
        auto oper = operator();
        auto rite = rite_to_left(with, pres);
        ensure_lvalue(left);
        assignment_op(left, rite, oper);
    }
    return to_rvalue(left);
}

value_t direct_load()
{
    reserve_slot();
    value_t value = {
        .type = {
            .name = { "i32" }
        },
        .name = number(),
        .is_rvalue = true,
        .slot = g_slot,
    };
    push(value);
    return value;
}

value_t indirect_load()
{
    auto name = string();
    auto found = lookup_value(name);
    if(found == nullptr)
    {
        quit_d("`%s` not defined", name.begin);
    }
    return *found;
}

value_t expr();

value_t paren_expr()
{
    match('(');
    auto value = expr();
    match(')');
    return value;
}

value_t p0()
{
    spaces();
    if(peek() == '(')
    {
        return paren_expr();
    }
    if(is_number_ch(peek()))
    {
        return direct_load();
    }
    if(is_string_ch(peek()))
    {
        return indirect_load();
    }
    quit_d("compiler error: unknown p0");
}

value_t p1()
{
    return left_to_rite(p0, 1);
}

value_t p2()
{
    return left_to_rite(p1, 2);
}

value_t p3()
{
    return left_to_rite(p2, 3);
}

value_t p4()
{
    return left_to_rite(p3, 4);
}

value_t p5()
{
    return left_to_rite(p4, 5);
}

value_t p6()
{
    return left_to_rite(p5, 6);
}

value_t p7()
{
    return rite_to_left(p6, 7);
}

value_t expr()
{
    return p7();
}

value_t expr_statement()
{
    auto value = expr();
    match(';');
    return value;
}

void ret_statement(value_t expected)
{
    auto value = expr_statement();
    type_check(value, expected);
    ret(value);
}

void declare_statement()
{
    auto left = declare();
    alloca(left);
    spaces();
    if(peek() == '=')
    {
        match('=');
        auto rite = expr();
        store(left, rite);
    }
    match(';');
}

void statement(value_t value)
{
    if(is_keyword(peek_keyword()))
    {
        auto key = keyword();
        if(str_equal(key, "ret"))
        {
            ret_statement(value);
        }
        else
        if(is_type(key))
        {
            step_back(key.size);
            declare_statement();
        }
        else
        {
            quit_d("unknown keyword `%s`", key.begin);
        }
    }
    else
    {
        expr_statement();
    }
}

void block(value_t value)
{
    int size = g_values_at;
    match('{');
    while(true)
    {
        spaces();
        if(peek() == '}')
        {
            break;
        }
        statement(value);
    }
    match('}');
    g_values_at = size;
}

void func()
{
    auto value = declare();
    define(value);
    emit_d("{");
    match('(');
    match(')');
    block(value);
    emit_d("}");
}

void program()
{
    while(!is_done())
    {
        func();
    }
}

void read_file(char* path)
{
    FILE* file = fopen(path, "r");
    fread(g_text, sizeof(char), g_file_size - 1, file);
    fclose(file);
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        quit_d("use: ./lang file.e");
    }
    read_file(argv[1]);
    target();
    program();
}
