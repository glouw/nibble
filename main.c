#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define quit_d(...) fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"), exit(1);
#define emit_d(...) fprintf(stdout, __VA_ARGS__), fprintf(stdout, "\n");

typedef struct
{
    char ch[32];
    int size;
}
string_t;

void append_s(string_t* self, char ch)
{
    self->ch[self->size++] = ch;
}

bool equal_s(string_t self, char* other)
{
    return strcmp(self.ch, other) == 0;
}

char* g_text;
char g_at;
int g_slot;

void bump()
{
    g_slot++;
}

char head()
{
    return g_text[g_at];
}

void next()
{
    g_at++;
}

void reverse(int by)
{
    g_at -= by;
}

bool is_space_ch(char ch)
{
    return ch == ' ' || ch == '\n' || ch == '\r';
}

bool is_operator_ch(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=';
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
    if(pres == 1) return equal_s(operator, "*") || equal_s(operator, "/");
    if(pres == 2) return equal_s(operator, "+") || equal_s(operator, "-");
    if(pres == 3) return equal_s(operator, "=");
}

bool is_keyword(string_t type)
{
    return equal_s(type, "ret");
}

void skip()
{
    while(is_space_ch(head()))
    {
        next();
    }
}

void match(char ch)
{
    skip();
    if(ch != head())
    {
        quit_d("expected `%c` but got `%c`", ch, head());
    }
    next();
}

string_t token(bool condition(char))
{
    skip();
    string_t out = {};
    while(condition(head()))
    {
        append_s(&out, head());
        next();
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

string_t ident()
{
    return string();
}

string_t keyword()
{
    return string();
}

string_t peek_keyword()
{
    auto k = keyword();
    reverse(k.size);
    return k;
}

string_t peek_operator()
{
    auto o = operator();
    reverse(o.size);
    return o;
}

typedef struct
{
    string_t string;
    bool is_lvalue;
    bool is_rvalue;
    int index;
    string_t type;
}
value_t;

value_t operate(value_t l, value_t r, string_t operator)
{
    bump();
    if(equal_s(operator, "=") && l.is_rvalue)
    {
        quit_d("expected lvalue, but `%s` was rvalue", l.string.ch);
    }
    if(equal_s(operator, "+")) emit_d("\t%%%d = add i32 %%%d, %%%d", g_slot, l.index, r.index);
    if(equal_s(operator, "-")) emit_d("\t%%%d = sub i32 %%%d, %%%d", g_slot, l.index, r.index);
    if(equal_s(operator, "*")) emit_d("\t%%%d = mul i32 %%%d, %%%d", g_slot, l.index, r.index);
    if(equal_s(operator, "/")) emit_d("\t%%%d = sdiv i32 %%%d, %%%d", g_slot, l.index, r.index);
    l.index = g_slot;
    return l;
}

value_t l2r(value_t with(), int pres)
{
    auto l = with();
    while(is_operator_s(peek_operator(), pres))
    {
        auto o = operator();
        auto r = with();
        l = operate(l, r, o);
    }
    return l;
}

value_t r2l(value_t with(), int pres)
{
    auto l = with();
    while(is_operator_s(peek_operator(), pres))
    {
        auto o = operator();
        auto r = r2l(with, pres);
        l = operate(l, r, o);
    }
    return l;
}

void direct_load(value_t value)
{
    emit_d("\t%%%d = add %s 0, %s", value.index, value.type.ch, value.string.ch);
}

value_t p0()
{
    bump();
    skip();
    value_t value;
    if(is_number_ch(head())) value = (value_t) { .string = number(), .is_rvalue = true, .type = "i32", .index = g_slot };
    if(is_string_ch(head())) value = (value_t) { .string = string(), .is_lvalue = true, .type = "i32", .index = g_slot };
    direct_load(value);
    return value;
}

value_t p1() { return l2r(p0, 1); }
value_t p2() { return l2r(p1, 2); }
value_t p3() { return r2l(p2, 3); }

value_t expr()
{
    return p3();
}

value_t expr_statement()
{
    auto value = expr();
    match(';');
    return value;
}

void block()
{
    match('{');
    while(true)
    {
        skip();
        if(head() == '}')
        {
            break;
        }
        if(is_keyword(peek_keyword()))
        {
            auto k = keyword();
            if(equal_s(k, "ret"))
            {
                auto value = expr_statement();
                emit_d("\tret %s %%%d", value.type.ch, g_slot);
            }
        }
        else
        {
            expr_statement();
        }
    }
    match('}');
}

void func()
{
    auto type = keyword();
    auto name = ident();
    emit_d("define %s @%s() {", type.ch, name.ch);
    match('(');
    match(')');
    block();
    emit_d("}");
}

int main(int argc, char* argv[])
{
    g_text = argv[1];
    func();
}
