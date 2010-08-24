/* This could easily be improved */
void *
memset (s, c, n)
     void *s;
     int c;
     unsigned int n;
{
  void *to = s;
  switch ( (unsigned int) s & 3 )
    {
    case 2:
    {
      short from ;
      from = (unsigned short) c | (unsigned short) c << 8 ;
      while ( (unsigned char *) to <= (unsigned char *) s + n - 2 )
	* ( (unsigned short *) to ) ++ = from ;
      break;
    }
    case 0:
    {
      unsigned int from ;
      from = (unsigned int) c | (unsigned int) c << 8;
      from = from | from << 16;
      while ( (unsigned char *) to <= (unsigned char *) s + n - 4 )
	* ( (unsigned int *) to ) ++ = from ;
    }
    default:
    }
  while ( (unsigned char *) to < (unsigned char *) s + n )
    * ( (unsigned char *) to ) ++ = c ;
  return s;
}
