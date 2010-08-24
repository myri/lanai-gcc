unsigned __umodsi3 (unsigned a, unsigned b)
{
  unsigned m;  /* A multiple of b */

  /* Subtract from "a" the largest multiple N*b of "b".
     Note than N*b can be expressed as
        N*b = c_0*2^0*b + c_1*2^1*b ... + c_max*2^max*b. */

  /* compute m=b*2^max */

  m = b;
  while (1)
  {
    unsigned  m2 = m * 2;
    if ((m2 > a) || (m & 0x80000000))
      break;
    else
      m = m2;
  }

  /* for i=max,max-1,...,0 , subtract c_i*2^i*b from a. */

  while (m >= b)
  {
    if (a >= m)
      a -= m;
    m >>= 1;
  }

  return a;
}
