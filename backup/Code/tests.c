#include <stdio.h>
#include <fonctio_a_tester.h>

#define TRUE 1
#define FALSE 0


//tester les fonction de types double

int test1(bool **principale, double **func) {
    
    if (principale(func) == true)
    {
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}

//tester les fonction de types int

int test2(bool **principale, int **func) {
    
    if (principale(func) == true)
    {
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}

//tester les fonction de types void

int test3(bool **principale, void **func) {
    
    if (principale(func) == true)
    {
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}



int main(int argc, char const *argv[])
{

	double d1 = 1;
	double d2 = 2;

	int x = 0;

	int *a;

	test1(principale(), MAX(d1, d2)); //TRUE

	test2(principale(), f(x)); //FALSE

	test3(principale(), f(a)); //FALSE


	return 0;
}