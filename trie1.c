#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
//#include"simplestTrie.h"
#define MAX_WORD_SIZE 50
typedef struct node
{
	struct node **arr;
	int next;
	int mean;
}node;
int memory;
int Max_Node_Size;
int p1,p2;
FILE *fp;
node *head;
int nextpos=0,meanpos=0;
int Count[27][27];
int totalNodes;
unsigned char *refTrie;
void insertTrie(char *s,int size)
{
	int i=0;
	node *curr = head;
	while(i<size)
	{
		int idx;
		//this wont affect.
		if(s[i]>='a' && s[i]<='z')
			idx = s[i]-'a';
		else
			idx = s[i]-'A';

		if(i==(size-1))
		{
			curr->mean = curr->mean | (1<<idx);
		}
		else
		{
			if((curr->next >> idx)&1)
			{
				curr = curr->arr[idx];
			}
			else
			{
				node *new = (node*)malloc(sizeof(node));
				new->arr = (node**)malloc(sizeof(node*)*26);
				new->next=0;
				new->mean=0;
				curr->arr[idx] = new;
				curr->next = curr->next | (1<<idx);
				curr = new;
			}
		}
		i++;
	}
}

/*int tsearch(char *word)
{
	int flg=0;
	node * curr = head;
	while( *word != '\0' && flg==0 )
	{
		putchar(*word);
		putchar('\n');
		if( (curr->next&( 1<<(*word - 'a'))) != 0)
		{
			curr = curr->arr[ *word - 'a' ];
		}
		else
		{
			putchar(*word);
			flg=1;
		}
		word++;
	}
	if( *(word) == '\0' &&  ( (curr->mean & ( 1 <<( *(word-1) - 'a'))) != 0))
	{
		putchar('a');
	}
	else
	{
		putchar('b');
	}
	return 0;
}*/

void createTrie()
{
	char word[MAX_WORD_SIZE];
	char c;
	int i=0;
	while((c=fgetc(fp))!=EOF)
	{
		if(c=='\n')
		{
			word[i]='\0';
			insertTrie(word,i);
		//	printf("%d words done\n",k++);
			i=0;
		}
		else
		{
			word[i]=c;
			i++;
		}
	}
}

int searchWord(char *word)
{
	int i=0,flg=0;
	node *curr = head;
	while(flg==0)
	{
//		putchar(word[i]);
//		putchar('\n');
		int idx;
		if(word[i]<='z' && word[i]>='a')
			idx = word[i]-'a';
		else
			idx = word[i]-'A';
	//	printf("in simple search %b %d\n",curr->mean,idx);
		if(word[i+1]==0 && ((curr->mean >> idx) & 1)==1)
		{
			flg=1;
//			putchar('p');
		}
		else if(word[i+1]!=0 && ((curr->next >> idx )&1)==1)
		{
			curr = curr->arr[idx];
			i++;
//			putchar('q');
		}
		else
		{
			flg=2;
//			putchar('r');
		}
	}
	return flg;
}
int returnCount(int a)
{
	int cnt=0;
	int val;
	while(a!=0)
	{
		cnt=cnt+(a & 1);
		a = a>>1;
	}
return cnt;
}

void generateStats(node *curr)
{
//	printf("%d\n",sizeof(*curr));
	totalNodes = totalNodes + 1;
//	if(curr->next==0 && curr->mean==0)
	int x = returnCount(curr->next);
	int y = returnCount(curr->mean);
/*	if(x==6 && y==6)
	{	printf("%d %d\n",x,y);
		printf("a %d %d\n",curr->next,curr->mean);
	}*/
	Count[x][y] = Count[x][y] + 1;
	for(int i=0;i<26;i++)
	{
		if(((curr->next>>i)&1)==1)
		{
			generateStats(curr->arr[i]);
		}
	}
	
}

void calNodesize(int a)
{
	p1 = ceil(log2(totalNodes));
	printf("p1: %d\n",p1);
	p2 = ceil(log2(a));
	int type0 = 0, type10 = 0, type11 = 0, type12 = 0, type20 = 0, type21 = 0, type3 = 0;
	for(int i=0;i<27;i++)
	{
		for(int j=0;j<27;j++)
		{
			if(i==26 && j==26)
				type0 = type0 + Count[i][j];
			else if(i==0 && j>=1 && j<=4)
				type10 = type10 + Count[i][j];
			else if(i>=1 && i<=4 && j>=1 && j<=4)
				type11 = type11 + Count[i][j];
			else if(j==0 && i>=1 && i<=4)
				type12 = type12 + Count[i][j];
			else if(i==0 && j>=5)
				type20 = type20 + Count[i][j];
			else if(j==0 && i>=5)
				type21 = type21 + Count[i][j];
			else
				type3 = type3 + Count[i][j];
		}
	}

		int m1 = 7*totalNodes + totalNodes*p1 + a*p2 + 2*totalNodes + 22*type10 + 42*type11 + 22*type12 + 27*(type20 + type21) + 52*type3;
		memory = ceil((float)m1/8);
		printf("%d\n",memory);
		while(ceil(log2(memory))>p1)
		{
			p1 = ceil(log2(memory));
			m1 = 7*totalNodes + totalNodes*p1 + a*p2 + 2*totalNodes + 22*type10 + 42*type11 + 22*type12 + 27*(type20 + type21) + 52*type3;
			memory = ceil((float)m1/8);
		}
		printf("mem: %d\n",memory);
}
/*
int ceil(int a,int b)
{
	if(a%b==0)
		return a/b;
	else
		return a/b+1;
}*/

void returnInfo(node *val,int nodeInfo[])
{
	int cnt1 = returnCount(val->next);
	int cnt2 = returnCount(val->mean);
	
	if(cnt1==26 && cnt2==26)
	{
		nodeInfo[0] = 0;
		nodeInfo[1] = ceil((float)(2 + 26*p1 + 26*p2)/8.0);
	}
	else if(cnt1==0 && cnt2>0 && cnt2<=4)
	{	
		nodeInfo[0] = 11;
		nodeInfo[1] = ceil((float)(2+2+20+cnt2*p2)/8.0);
	}
	else if(cnt1>0 && cnt1<=4 && cnt2>0 && cnt2<=4)
	{	
		nodeInfo[0] = 12;
		nodeInfo[1] = ceil((float)(2+2+20+20+cnt1*p1+cnt2*p2)/8.0);
	}
	else if(cnt1>0 && cnt1<=4 && cnt2==0)
	{
		nodeInfo[0] = 13;
		nodeInfo[1] = ceil((float)(2+2+20+cnt1*p1)/8.0);
	}
	else if(cnt1==0 && cnt2>=5)
	{
		nodeInfo[0] = 21;
		nodeInfo[1] = ceil((float)(2+1+26+cnt2*p2)/8.0);
	}
	else if(cnt1>=5 && cnt2==0)
	{	
		nodeInfo[0] = 22;
		nodeInfo[1] = ceil((float)(2+1+26+cnt1*p1)/8.0);
	}
	else
	{	
		nodeInfo[0] = 3;
		nodeInfo[1] = ceil((float)(2+26+26+cnt1*p1+cnt2*p2)/8.0);
	}

}

void setbit(int value, unsigned char *location,int position,int pointer_size)
{
	/*if (pointer_size == 1)
	{
		printf("yrr %d %d\n",position,value);
	}*/
	int x = position + pointer_size - 1,i=0;
	while(x>=position)
	{
		//printf("in set bit :  %d %d\n",x/8,x%8);
		location[x/8] = location[x/8] | ((value>>i)&1)<<(7 - (x % 8));
		/*if(pointer_size == 1)
		{
			printf("arre %x\n",location[x/8]);
		}*/
		i++;
		x--;
	}
}

void createReference(node *curr)//,char* meaningFile)
{
	//FILE *p = fopen(meaningFile,"r");
	//FILE *q = fopen(refData,"w");
	int nodeInfo[2];
	returnInfo(curr,nodeInfo);
	//printf("In create ref %d %d\n",nodeInfo[0],nodeInfo[1]);
	int curpos=nextpos;
	nextpos = nextpos + nodeInfo[1];
	//printf("next pos %d\n",nextpos);
	unsigned char content[nodeInfo[1]];
	memset(content,0,nodeInfo[1]);
	int bitswritten = 0;
	if(nodeInfo[0]==0)
	{
		setbit(0,content,bitswritten,2); //value,location,position,places
		bitswritten = bitswritten + 2;
	}
	else if(nodeInfo[0]==11)
	{
		setbit(4,content,bitswritten,4);
		bitswritten = bitswritten + 4;
		int i = 0, temp = curr->mean, k=0;
		while(i<26)
		{
			if((temp >> (i))&1)
			{
				setbit(i,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
			i++;
		}
		if(k<4)
		{
			while(k<4)
			{
				setbit(26,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
		}
	}
	else if(nodeInfo[0]==12)
	{
		//printf("here\n");
		setbit(5,content,bitswritten,4);
		//printf("%x",content[0]);
		bitswritten = bitswritten + 4;
		int i = 0, temp = curr->next, k=0;
		while(i<26)
		{
			if((temp >> (i))&1)
			{
				//printf("i: %d\n",i);
				setbit(i,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
			i++;
		}
		if(k<4)
		{
			while(k<4)
			{
				setbit(26,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
		}
		i = 0, temp = curr->mean, k=0;
		while(i<26)
		{
			if((temp >> (i))&1)
			{
				setbit(i,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
			i++;
		}
		if(k<4)
		{
			while(k<4)
			{
				setbit(26,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
		}
	}
	else if(nodeInfo[0]==13)
	{
		setbit(6,content,bitswritten,4);
		bitswritten = bitswritten + 4;
	
		int i = 0, temp = curr->next, k=0;
		while(i<26)
		{
			if((temp >> (i))&1)
			{
				setbit(i,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
			i++;
		}
		if(k<4)
		{
			while(k<4)
			{
				setbit(26,content,bitswritten,5);
				bitswritten = bitswritten + 5;
				k++;
			}
		}
	}
	else if(nodeInfo[0]==21)
	{
		setbit(4,content,bitswritten,3);
		bitswritten = bitswritten + 3;

		setbit(curr->mean,content,bitswritten,26);
		bitswritten = bitswritten + 26;
	}
	else if(nodeInfo[0]==22)
	{
		setbit(5,content,bitswritten,3);
		bitswritten = bitswritten + 3;

		setbit(curr->next,content,bitswritten,26);
		bitswritten = bitswritten + 26;
	}
	else if(nodeInfo[0]==3)
	{
		setbit(3,content,bitswritten,2);
		bitswritten = bitswritten + 2;

		setbit(curr->next,content,bitswritten,26);
		bitswritten = bitswritten + 26;

		setbit(curr->mean,content,bitswritten,26);
		bitswritten = bitswritten + 26;
	}
/*	if(nodeInfo[0]==3)
	{
		printf("printing bitmaps: %b %b %b %b %b\n",content[0],content[1],content[2],content[3],content[4]);
	}
*/
	int cnt = returnCount(curr->next), t = bitswritten;
	for(int i=0;i<26;i++)
	{
		if((curr->mean>>i)&1)
		{
		//	printf("meanpos %d\n",meanpos);
			setbit(meanpos,content,(t + cnt*p1),p2);
			t = t + p2;
			meanpos = meanpos + 1;
		}
	}


	for(int i=0;i<26;i++)
	{
		if((curr->next>>i)&1)
		{
			setbit(nextpos,content,bitswritten,p1);
			bitswritten = bitswritten + p1;
			createReference(curr->arr[i]);
		}
	}

/*	for(int i=0;i<26;i++)
	{
		if((curr->mean>>i)&1)
		{
		//	printf("meanpos %d\n",meanpos);
			setbit(meanpos,content,bitswritten,p2);
			bitswritten = bitswritten + p2;
			meanpos = meanpos + 1;
		}
	} */

	for(int i=0;i<nodeInfo[1];i++)
	{
		refTrie[curpos + i] = content[i];	
	}
}

int extract(int pos, unsigned char arr[],int bits)
{
	int a = 0,i=pos;
	int total = bits + pos -1;
	while(i <= total)
	{
		a = a | ((arr[i/8]>>(7-(i % 8)))&1);
		if(i!=total)
			a = a << 1;
	//	printf("%b\n",a);
		i++;
	/*	if(bits==5)
		{
			printf("in extract %d\n",a);
		}*/
	}
	//printf("a: %d\n",a);
	return a;
}

void calnodeStats(int stats[],unsigned char arr[])
{
//	printf("stats %x %x \n",arr[0],arr[1]);
	stats[0] = arr[0] >> 6;

	if(stats[0]==0 || stats[0]==3)
		stats[1]=-1;
	else if(stats[0]==1)
	{
		stats[1] = arr[0] >> 4;
	/*	printf("%d\n",stats[1]);
		stats[1] = stats[1] << 6;
		printf("%d\n",stats[1]);
		stats[1] = stats[1] >> 6;
		printf("%d\n",stats[1]); */
		stats[1] = stats[1] & 3;
	//	printf("sub type : %d\n",stats[1]);
	}
	else if(stats[0]==2)
	{
		stats[1] = arr[0] >> 5;
		/*stats[1] = stats[1] << 7;
		stats[1] = stats[1] >> 7;*/
		stats[1] = stats[1] & 1;

	//	printf("sub type : %d\n",stats[1]);
	}

	switch(stats[0])
	{
	case 0:
	{
		stats[3] = -1;
		stats[4] = -1;
		stats[2] = ceil((float)(2 + 26*p1 + 26*p2)/8.0);
		break;
	}
	case 1:
	{	
		if(stats[1]==0)
		{	
			int a,cnt2=0;
			for(int i=0;i<4;i++)
			{
			 	a = extract(i*5+4,arr,5); // pos, location, number of bits to be extracted
				if(a<26)
					cnt2++;
			}
			stats[2] = ceil((float)(2+2+20+cnt2*p2)/8.0);
			stats[3] = -1;
			stats[4] = -1;
		}
		else if(stats[1]==1)
		{	
			//printf("i am here\n");
			int a,cnt1=0,cnt2=0;
			for(int i=0;i<4;i++)
			{
				a = extract(i*5+4,arr,5);
				//printf("a is : %d\n",a);
				if(a<26)
					cnt1++;
			}
			for(int i=0;i<4;i++)
			{
				a = extract(i*5+24,arr,5);
				if(a<26)
					cnt2++;
			}
			stats[2] = ceil((float)(2+2+20+20+cnt1*p1+cnt2*p2)/8.0);
			stats[3] = -1;
			stats[4] = -1;
		}
		else if(stats[1]==2)
		{
			int a, cnt1=0;
			for(int i=0;i<4;i++)
			{
				a = extract(i*5+4,arr,5);
				if(a<26)
					cnt1++;
			}
			stats[2] = ceil((float)(2+2+20+cnt1*p1)/8.0);
			stats[3] = -1;
			stats[4] = -1;
		}
		break;
	}
	case 2:
	{
		if(stats[1]==0)
		{
			int a = extract(3,arr,26);
			stats[3] = -1;
			stats[4] = a;
			int cnt2=0;
			while(a!=0)
			{
				cnt2 = cnt2 + (a & 1);
				a = a>>1;
			}
			stats[2] = ceil((float)(2+1+26+cnt2*p2)/8.0);
		}
		else if(stats[1]==1)
		{	
			int a = extract(3,arr,26);
			stats[3] = a;
			stats[4] = -1;
			int cnt1 = 0;
			while(a!=0)
			{
				cnt1 = cnt1 + (a & 1);
				a = a>>1;
			}
			stats[2] = ceil((float)(2+1+26+cnt1*p1)/8.0);
		}
		break;
	}
	case 3:
	{	
		int a = extract(2,arr,26);
		stats[3] = a;
		int cnt1 = 0;
		while(a!=0)
		{
			cnt1 = cnt1 + (a & 1);
			a = a >> 1;
		}
		a = extract(28,arr,26);
		stats[4] = a;
		int cnt2 = 0;
		while(a!=0)
		{
			cnt2 = cnt2 + (a & 1);
			a = a >> 1;
		}
		stats[2] = ceil((float)(2+26+26+cnt1*p1+cnt2*p2)/8.0);
		break;
	}
}
}

int newTrieSearch(char *word)
{
	int ptr = 0,i=0, result = 2;
	//printf("hello\n");
	while(word[i]!='\0' && result==2)
	{
	//	putchar(word[i]);
	//	putchar('a');
	//	putchar('\n');
	//	printf("%x val\n",word[i]);
		int idx;
		if(word[i]>='a' && word[i]<='z')
			idx = word[i] - 'a';
		else
			idx = word[i] - 'A';

		//printf("index for %c is %d",word[i],idx);

	//	putchar('a');
	//	putchar('\n');
		unsigned char a[Max_Node_Size];
		int k = 0;
		for(int j=ptr;j<(ptr + Max_Node_Size);j++)
		{
			a[k]=refTrie[j];
			k++;
		}
	/*	for(int m=0;m<Max_Node_Size;m++)
		{
			printf("%x\t",a[m]);
		}*/
	//	putchar('\n');

	//	putchar('a');
	//	putchar('\n');
		int nodeStats[5]; //0- type 1- subtype 2- node size 3- bitmap1  4- bitmap2
		memset(nodeStats,0,5*4);
		calnodeStats(nodeStats,a);

	//	putchar('a');
	//	putchar('\n');
		//printf("%d %d %d %d %d\n",nodeStats[0],nodeStats[1],nodeStats[2],nodeStats[3],nodeStats[4]);
	switch(nodeStats[0])
	{
		case 0:
		{
			if(word[i+1]=='\0')
			{
				result = 1;
			}
			else
			{
				ptr = extract(idx*p1+2,a,p1);
			}
			break;
		}
		case 1:
		{
			switch(nodeStats[1])
			{
				case 0:
				{
					//printf("%x %x\n",word[i],word[i+1]);
					if(word[i+1]=='\0')
					{
						//printf("hello there\n");
						int ex, j = 0;
						char b = 0;
						while(j<4 && b==0)
						{
							ex = extract(j*5+4,a,5);
							//printf("ex: %d\n",ex);
							if(ex==idx)
							{
								b = b | 1;
								result = 1;
							}
							else
								j=j+1;
						}
						if(j==4)
							result = 0;
					}
					else
						result = 0;
					break;
				}
				case 1:
				{
					if(word[i+1]=='\0')
					{
						int ex, j = 0;
						char b = 0;
						while(j<4 && b==0)
						{
							ex = extract(j*5+24,a,5);
							if(ex==idx)
							{
								b = b | 1;
								result = 1;
							}
							else
								j=j+1;
						}
						if(j==4)
							result = 0;
					}
					else
					{
						int ex,j=0;
						char b = 0;
						while(j<4 && b==0)
						{
							ex = extract(j*5+4,a,5);
							if(ex==idx)
							{
								b = b | 1;
							}
							else
								j=j+1;
						}
						if(j==4)
							result = 0;
						else
							ptr = extract(j*p1+44,a,p1);
							//printf("ptr : %d\n",ptr);
					}

					break;
				}
				case 2:
				{
					//printf("very important good morning message\n");
					if(word[i+1]=='\0')
						result = 0;
					else
					{
						int ex,j=0;
						char b = 0;
						while(j<4 && b==0)
						{
							ex = extract(j*5+4,a,5);
							if(ex==idx)
							{
								b = b | 1;
							}
							else
								j=j+1;
						}
					if(j==4)
						result = 0;
					else
						ptr = extract(j*p1+24,a,p1);
					//printf("very important get out message: %d\n",ptr);
					}
					break;
				}
			}
			break;
		}
		case 2:
		{
			switch(nodeStats[1])
			{
				case 0:
				{
					if(word[i+1]=='\0' && ((nodeStats[4]>>idx)&1)==1)
						result = 1;
					else
						result = 0;
					break;
				}
				case 1:
				{
					if(word[i+1]=='\0')
						result = 0;
					else
					{
						if(((nodeStats[3]>>idx)&1)==1)
						{
							int cnt = 0;
							for(int k=0;k<idx;k++)
								cnt = cnt + ((nodeStats[3]>>k)&1);
							ptr = extract(cnt*p1+3+26,a,p1);
						}
						else
							result = 0;
					}
					break;
				}
			}
			break;
		}
		case 3:
		{
			//printf("case 3 ptr : %d\n",ptr);
			if(word[i+1]=='\0' && ((nodeStats[4]>>idx)&1)==1)
			{
				result = 1;
				//printf("bhagwan hai kaha re tu\n");
			}
			else if(word[i+1]!='\0' && ((nodeStats[3]>>idx)&1)==1)
			{
				//printf("yaha hu mai\n");
				int cnt = 0;
				for(int k=0;k<idx;k++)
					cnt = cnt + ((nodeStats[3]>>k)&1);
				ptr = extract(cnt*p1+2+26+26,a,p1);
				//printf("%d\n",ptr);
			}
			else
				result = 0;
			//printf("hello\n");
			break;
		}
	}
		i = i+1;
	//	putchar('a');
	//	putchar('\n');
	}

	//	putchar('a');
	//	putchar('\n');
return result;
}


int main(int x,char *file[])
{
	fp = fopen(file[1],"r");
	printf("size of node : %d\n",sizeof(node));
	head = (node*)malloc(sizeof(node));
	head->arr = (node**)malloc(sizeof(node*)*26);
	head->next = 0;
	head->mean = 0;
	createTrie();
//	createMeaningfile(atoi(file[2]));
	fclose(fp);
/*	time_t t1 = time(NULL);
	for(int j=0;j<atoi(file[2]);j++)
	{
	char word[50];
	scanf("%s",word);
	int i = searchWord(word);
	switch(i)
	{
		case 1: printf("found\n");
			break;
		case 2: printf("word not found\n");
			break;
	}
	word[0] = '\0';
	}
	time_t t2 = time(NULL);
	printf("time : %f\n",t2-t1);*/
	memset(Count,0,4*27*27);
	/*for(int i=1;i<=10;i++)
	{
		char s[3];
		sprintf(s,"p%d",i);
		printf("%s %d\n",s,s);
	}*/
//	printf("%b %b %b %b %b %b %b %b %b %b \n",p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
	generateStats(head);
/*	for(int i=0;i<27;i++)
	{
		for(int j=0;j<27;j++)
		{
			printf("%d\t",Count[i][j]);
		}
		printf("\n");
	} */
	printf("Total Nodes : %d\n",totalNodes);
	//calNodesize();
	//printf("tot: %d\n",cnt);
	p1=ceil(log2(totalNodes)),p2=ceil(log2(atoi(file[2])));
	//int temp = totalNodes;
	/*while(temp!=0)
	{
		p1 = p1+1;
		temp= temp>>1;
	}
	temp = atoi(file[2]);
	while(temp!=0)
	{
		p2=p2+1;
		temp=temp>>1;
	}*/
	Max_Node_Size = 2+26+26+25*p1+25*p2;
//	int memory = ceil((Max_Node_Size * totalNodes)/8); // in bits
/*	if((memory%8)==0) // converting to bytes by taking ceiling value
		memory = memory/8; 
	else
		memory = memory/8 + 1;
*/
/*	while(ceil(log2(memory))>p1)
	{
		p1 = ceil(log2(memory));
		Max_Node_Size = 2 + 26 + 26 + 25*p1 + 25*p2;
		memory = ceil((float)(Max_Node_Size*totalNodes)/8.0);
	}*/
	calNodesize(atoi(file[2]));
	printf("memory : %d np: %d mp: %d\n",memory,p1,p2);
	refTrie = (unsigned char*)malloc(sizeof(unsigned char)*memory);
	memset(refTrie,0,memory);
	createReference(head);//,file[3]);
//	printf("reference: %x %x %x %x %x %x %x %x\n",refTrie[264871],refTrie[264872],refTrie[264873],refTrie[264874],refTrie[264875],refTrie[264876],refTrie[264877],refTrie[264878]);
/*	for(int i=0;i<memory;i++)
	{
		printf("%2x\t",refTrie[i]);
	}
	putchar('\n');
	int info[2];
	returnInfo(head->arr[1],info);
	printf("type %d size %d\n",info[0],info[1]);*/
//	unsigned char a[1];
//	a[0] = 138;
//	extract(1,a,5);
	char s[MAX_WORD_SIZE];
	time_t ti = time(NULL);
	for(int i=0;i<atoi(file[2]);i++)
	{
		scanf("%s",s);
//		printf("word is %s\n",s);
		int res = newTrieSearch(s);
		/*if(res==0)
		{
			printf("not found\n");
			break;
		}
		else
			printf("found\n");*/
		s[0] = '\0';
	}

	time_t t2 = time(NULL);
	printf("time %d\n",t2-ti);
}
