#include<stdio.h>
#include<stdlib.h>
#include<string.h>

union pp{
	int i;
	char tr[3];
	};

int main(){
	int i=0xaabbccdd;
	unsigned char arr[4];
	memset(arr,0,4);
	arr[3]=0xee;
	union pp t;
	
	

	t.i=0xaabbccdd;
	memcpy(arr,t.tr,3);
	
	union pp q;
	q.i=0;
	memcpy(q.tr,arr,3);
	union pq{ int i;char arr[3];} v1;	
	v1.i=-1;
	q.i=0;
	memcpy(q.tr,v1.arr,3);
	printf("%x",q.i);
	printf("%x",-1);

}
