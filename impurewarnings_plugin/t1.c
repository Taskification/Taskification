#include <stdio.h>

int global_variable = 99;

int ft1( )
{
	return global_variable ;
}


int ft2()
{
	global_variable = 9;

}

int ft3()
{
	int a,b ;
	a= b + global_variable ;

}

int ft4()
{
	int a= global_variable ;

}
int main(int arcg, char ** argv)
{
	ft1();
	ft2();
	ft3();
	ft4();
	return 0;

}
