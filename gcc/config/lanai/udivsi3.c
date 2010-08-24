unsigned int
__udivsi3 (a, b)
    register unsigned int a, b;
{
    register unsigned val, mask;
    if(!(a|b)) return(0);
    mask = 1;
    while((int)(b) > 0) { b <<= 1; mask <<= 1; }
    while((int)(a) > 0) { a <<= 1; mask >>= 1; }
    for(val = 0; ; )
    {
	if(a >= b) { a -= b; val |= mask; }
	b >>= 1; if(!(mask >>= 1)) return(val);
    }
}
