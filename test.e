i32 main()
{
    i32 x;
    x = 32;

    i32 value;
    value = 32 + 2 * x / 2;

    i32* p;
    p = &value;

    i32** pp;
    pp = &p;

    i32*** ppp;
    ppp = &pp;

    ***ppp = value - 64;

    ret value;
}
