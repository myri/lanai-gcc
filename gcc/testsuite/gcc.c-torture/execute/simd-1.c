/* Origin: Aldy Hernandez <aldyh@redhat.com>

   Purpose: Test generic SIMD support.  This test should work
   regardless of if the target has SIMD instructions.
*/

typedef int __attribute__((mode(V4SI))) vecint;

vecint i = { 150, 100, 150, 200 };
vecint j = { 10, 13, 20, 30 };
vecint k;

union {
  vecint v;
  int i[4];
} res;

/* This should go away once we can use == and != on vector types.  */
void
verify (int a1, int a2, int a3, int a4,
	int b1, int b2, int b3, int b4)
{
  if (a1 != b1
      || a2 != b2
      || a3 != b3
      || a4 != b4)
    abort ();
}

int
main ()
{
  k = i + j;
  res.v = k;

  verify (res.i[0], res.i[1], res.i[2], res.i[3], 160, 113, 170, 230);

  k = i * j;
  res.v = k;

  verify (res.i[0], res.i[1], res.i[2], res.i[3], 1500, 1300, 3000, 6000);

  k = i / j;
  res.v = k;

  verify (res.i[0], res.i[1], res.i[2], res.i[3], 15, 7, 7, 6);

  k = -i;
  res.v = k;
  verify (res.i[0], res.i[1], res.i[2], res.i[3],
	  -150, -100, -150, -200);

  exit (0);
}