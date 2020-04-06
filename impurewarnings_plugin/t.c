#include <stdio.h>

int global_variable = 99;

int pure(void)
{
	return 5;
}

int impure()
{	
	global_variable = 9;
	printf("LL\n");
	return 0;
}

int main(int arcg, char ** argv)
{
	pure();
	impure();
	return 0;
}
