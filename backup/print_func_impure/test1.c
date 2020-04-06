#include<stdlib.h>
int glob = 0 ;

int ft_pure () {
int  a =23;
int b= 1 ;
return a + b ;
}

int ft_non_pure1 () {
  glob = 5 ;
}

int ft_non_pure2 () {
    void *ptr = malloc(20) ;
    return 0 ;
}


int main(int argc, char const *argv[]) {
   ft_pure() ;
   ft_non_pure1() ;
   ft_non_pure2() ; 
  return 0;
}
