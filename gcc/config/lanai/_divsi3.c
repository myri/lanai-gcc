int
__divsi3 (a, b)
    int a, b;
{
    if(a < 0) { if(b < 0) { return( __udivsi3(-a,-b)); }
		     else { return(-__udivsi3(-a, b)); } }
	 else { if(b < 0) { return(-__udivsi3( a,-b)); }
		     else { return( __udivsi3( a, b)); } }
}
