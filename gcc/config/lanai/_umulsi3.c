unsigned int
__umulsi3 (a, b)
    register unsigned int a, b;
{
    register result=0;
    do{
	if(a&1){
	    result += b;
	}
	a>>=1;
	b<<=1;
    }while(a|b);
    return result;
}
