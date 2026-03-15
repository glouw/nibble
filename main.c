#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr int g_file_size = 4096;
constexpr int g_string_size = 32;
constexpr int g_values_size = 128;

typedef struct
{
    char ch[g_string_size];
    int size;
}
string_t;

typedef struct
{
    string_t name;
    bool is_lvalue;
    bool is_rvalue;
    int slot;
    string_t type;
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

void append_s(string_t* self, char ch)
{
    self->ch[self->size] = ch;
    self->size += 1;
}

bool streq(string_t self, char* other)
{
    return strcmp(self.ch, other) == 0;
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
    return ch == ' ' || ch == '\n' || ch == '\r';
}

bool is_operator_ch(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '<' || ch == '>';
}

bool is_lower_ch(char ch)
{
    return ch >= 'a' && ch <= 'z';
}

bool is_upper_ch(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

bool is_number_ch(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool is_string_ch(char ch)
{
    return is_lower_ch(ch) || is_upper_ch(ch) || is_number_ch(ch);
}

bool is_operator_s(string_t operator, int pres)
{
    if(pres == 1) return streq(operator, "*") || streq(operator, "/");
    if(pres == 2) return streq(operator, "+") || streq(operator, "-");
    if(pres == 3) return streq(operator, "<<") || streq(operator, ">>");
    if(pres == 4) return streq(operator, "=");
    quit_d("compiler error: unknown operator precedence and/or operator");
}

bool is_type(string_t type)
{
    return streq(type, "i32");
}

bool is_keyword(string_t keyword)
{
    return streq(keyword, "ret") || is_type(keyword);
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
        append_s(&out, peek());
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

value_t* lookup_value(string_t name)
{
    for(int i = 0; i < g_values_at; i++)
    {
        auto value = &g_values[i];
        if(streq(value->name, name.ch))
        {
            return value;
        }
    }
    return nullptr;
}

string_t ident(string_t type)
{
    auto name = string();
    if(lookup_value(name))
    {
        quit_d("`%s` already defined", name.ch);
    }
    g_values[g_values_at] = (value_t) {
        .name = name,
        .is_lvalue = true,
        .type = type,
    };
    g_values_at += 1;
    return name;
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

value_t to_rvalue(value_t value)
{
    if(value.is_lvalue)
    {
        reserve_slot();
        value.slot = g_slot;
        emit_d("\t%%%d = load %s, %s* %%%s", value.slot, value.type.ch, value.type.ch, value.name.ch);
        value.is_lvalue = false;
        value.is_rvalue = true;
    }
    return value;
}

void ensure_lvalue(value_t value)
{
    if(value.is_rvalue)
    {
        quit_d("`%s` must be lvalue", value.name.ch);
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
        if(streq(oper, "+")) emit_d("\t%%%d = add %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);
        if(streq(oper, "-")) emit_d("\t%%%d = sub %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);
        if(streq(oper, "*")) emit_d("\t%%%d = mul %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);
        if(streq(oper, "<<")) emit_d("\t%%%d = shl %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);

        // These need unsigned counterparts...

        if(streq(oper, "/")) emit_d("\t%%%d = sdiv %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);
        if(streq(oper, ">>")) emit_d("\t%%%d = ashr %s %%%d, %%%d", g_slot, left.type.ch, left.slot, rite.slot);

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
        if(streq(oper, "=")) emit_d("\tstore %s %%%d, %s* %%%s", rite.type.ch, rite.slot, left.type.ch, left.name.ch);
    }
    return to_rvalue(left);
}

value_t direct_load()
{
    reserve_slot();
    value_t value = {
        .name = number(),
        .is_rvalue = true,
        .type = { "i32" },
        .slot = g_slot,
    };
    emit_d("\t%%%d = add %s 0, %s", value.slot, value.type.ch, value.name.ch);
    return value;
}

value_t indirect_load()
{
    auto name = string();
    auto found = lookup_value(name);
    if(found == nullptr)
    {
        quit_d("`%s` not defined", name.ch);
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

value_t p1() { return left_to_rite(p0, 1); }
value_t p2() { return left_to_rite(p1, 2); }
value_t p3() { return left_to_rite(p2, 3); }
value_t p4() { return rite_to_left(p3, 4); }

value_t expr()
{
    return p4();
}

value_t expr_statement()
{
    auto value = expr();
    match(';');
    return value;
}

void ret_statement()
{
    auto value = expr_statement();
    emit_d("\t%s %s %%%d", "ret", value.type.ch, value.slot);
}

void decl_statement(string_t type)
{
    auto name = ident(type);
    emit_d("\t%%%s = alloca %s", name.ch, type.ch);
    match(';');
}

void statement()
{
    if(is_keyword(peek_keyword()))
    {
        auto key = keyword();
        if(streq(key, "ret"))
        {
            ret_statement();
        }
        if(streq(key, "i32"))
        {
            decl_statement(key);
        }
    }
    else
    {
        expr_statement();
    }
}

void block()
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
        statement();
    }
    match('}');
    g_values_at = size;
}

void func()
{
    auto type = keyword();
    auto name = ident(type);
    emit_d("define %s @%s() {", type.ch, name.ch);
    match('(');
    match(')');
    block();
    emit_d("}");
}

void program()
{
    while(!is_done())
    {
        func();
    }
}

void emit_header()
{
    emit_d("target triple = \"x86_64-pc-linux-gnu\"\n");
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
    emit_header();
    program();
}
