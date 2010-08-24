/* This could easily be improved */
char *
memcpy (s, ct, n)
char *s;
const char *ct;
unsigned int n;
{
  void *to = s;
  const void *from = ct;
  switch ( ( (unsigned int) s | n ) & 3 )
    {
    case 2:
    {
      while ( (unsigned char *) to <= (unsigned char *) s + n - 2 )
	* ( (unsigned short *) to ) ++
	  = * ( (const unsigned short *) from ) ++ ;
      break;
    }
    case 0:
    {
      while ( (unsigned char *) to <= (unsigned char *) s + n - 4 )
	* ( (unsigned int *) to ) ++
	  = * ( (const unsigned int *) from ) ++ ;
    }
    default:
    }
  while ( (unsigned char *) to < (unsigned char *) s + n )
    {
      * ( (unsigned char *) to ) ++
	  = * ( (const unsigned char *) from ) ++ ;
    }
  return s;
}
