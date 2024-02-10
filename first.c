#include<stdio.h>



int main(int argc,char *argv[])
{
	unsigned char store[1024*1024];
	FILE * p = fopen(argv[1],"r");
	if( p != NULL )
	{
		while(

