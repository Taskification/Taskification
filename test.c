int f();              // Declaration without prototype.
int f(int x, int y);  // Declaration with prototype.

void g(int x);
void g(int);  // Unnamed parameter.



/* in the .h file */
int divide(int numerator, int denominator);

/* in the .c file */
int divide(int denominator, int numerator)
{
	return numerator / denominator;
}
