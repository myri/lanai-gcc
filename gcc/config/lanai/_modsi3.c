int
__modsi3 (a, b)
    int a, b;
{
  if (a < 0) { if (b < 0) { return (-__umodsi3 (-a, -b)); }
                     else { return (-__umodsi3 (-a,  b)); } }
        else { if (b < 0) { return ( __umodsi3 ( a, -b)); }
                     else { return ( __umodsi3 ( a,  b)); } }
}
