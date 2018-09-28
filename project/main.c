#include<stdio.h>
#include "print.h"
int main(void){
	printhello();
	int *p;
	int a;
	scanf("%d", &a);
	if(a > 1){
		p = &a;
	}
	*p = 0;
	return *p;
}
