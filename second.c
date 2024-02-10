/*
   A simple program to extract the statistical mesures empirically from
   the trie representation of a word lisdt for purpose of optimising the 
   final representation of the tree.

   some constraints
   words -> [a-z]+


   compile with gcc or clang dosnt matter.
   $cc second.c -o second
   for invocation
   $./second wordlist outputfilename

 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define ALPHA_SIZE 26
#define FALSE 0
#define TRUE 1
#define MAX_L 1024
#define T1 0
#define T2 1
#define T3 2

int max_l=0;
int max_m=0;
int global_index=0;
unsigned char pp[512*1024*1024];
typedef struct node
{
	struct node * nnode[ALPHA_SIZE];
	int meanPos[ALPHA_SIZE];
	int meaning;
}node;




node * node_gen()
{
	node * temp = (node *)malloc(sizeof(node));
	temp->meaning = 0;
	memset(temp->meanPos,0,sizeof(int)*ALPHA_SIZE);
	memset(temp->nnode,0,sizeof(NULL)*ALPHA_SIZE);
	return temp;
}


void add_to_trie(char * word,int mean,node * root)
{
	node * temp = root;
	while( *(word+1)!='\0' )
	{
		if( temp->nnode[ *word - 'a' ] == NULL )
		{
			temp->nnode[ *word - 'a' ] = node_gen();
		}
		temp = temp->nnode[ *word - 'a' ];
		word=word+1;
	}
	temp->meanPos[(*word)-'a']=mean;
	temp->meaning = (temp->meaning | (1<<( *(word) - 'a'))) ;
}

int srch(char * word,node * root)
{
	int ret = -1;
	node * temp = root;
	while( *(word+1) != '\0' && temp!=NULL )
	{
		temp = temp->nnode[ *word - 'a' ];
		word=word+1;
	}
	if( temp != NULL && ( (temp->meaning & ( 1 << (*(word) - 'a'))) == ( 1 << (*(word) - 'a'))))
	{
		ret = temp->meanPos[*word - 'a'];
	}
	return ret;
}

void count_nodes(node * root,int * arr)
{
	if( root != NULL )
	{
		int lc=0;
		int mc=0;
		
		for( int i=0;i<ALPHA_SIZE;i++)
		{
			if( root->nnode[i] != NULL )
			{
				lc++;
				count_nodes(root->nnode[i],arr);
			}
			if( (root->meaning & ( 1<<i )) != 0 )
			{
				mc++;
			}
		}
		if( lc > max_l ) max_l = lc;
		if( mc > max_m ) max_m = mc;
		arr[lc*ALPHA_SIZE + mc]++;
	}
}

int nodeType(node * curr)
{
	int nxt=0;
	int mng=0;
	int ret=-1;
	for( int i=0;i<ALPHA_SIZE;i++)
	{
		if( curr->nnode[i] != NULL )
		{
			nxt++;
		}
		if( (curr->meaning & ( 1<<i )) != 0 )
		{
			mng++;
		}
	}
	if( mng == 0 && nxt != 0 )
	{
		ret=0;
	}
	else if( mng != 0 && nxt==0 )
	{
		ret=1;
	}
	else if(mng!=0 && nxt!=0)
	{
		ret=2;
	}
	return ret;
}

int flat_gen(node *root)
{
	int ret_address = global_index;
	int local_index = global_index;
	int n_type = nodeType(root);
	union valset{ int i;char lcr[3];}v1;
	if( n_type == 0 )	// 1+26*4=105b
	{
		global_index+=105;
		
		pp[local_index]=T1;
		local_index+=1;
		for( int i=0;i<ALPHA_SIZE;i++)
		{
			if( root->nnode[i] != NULL)
				*((int *)(pp + local_index)) = flat_gen(root->nnode[i]);
			else
				*((int *)(pp + local_index)) = -1; 
			local_index+=4;

		}

	}
	else if( n_type == 1 ) // 1+26*3=79
	{
		global_index+=79;
		pp[local_index]=T2;
		local_index+=1;
		for( int i=0;i<ALPHA_SIZE;i++)
		{
			v1.i=-1;
			if( (root->meaning & ( 1<<i )) != 0 )
			{
				v1.i = root->meanPos[i];
				memcpy(pp+local_index,v1.lcr,3);
			}
			memcpy(pp+local_index,v1.lcr,3);
			local_index+=3;
		}
	}
	else if( n_type == 2 ) // 1+26*4+26*3= 76+104+1 = 181
	{
		global_index+=181;
		pp[local_index]=T3;
		local_index+=1;
		for( int i=0;i<ALPHA_SIZE;i++)
		{
			if( root->nnode[i] != NULL)
			*((int *)(pp + local_index)) = flat_gen(root->nnode[i]);
			local_index+=4;
		}
		for( int i=0;i<ALPHA_SIZE;i++)
		{
			v1.i=-1;
			if( (root->meaning & ( 1<<i )) != 0 )
			{
				v1.i = root->meanPos[i];
				memcpy(pp+local_index,v1.lcr,3);
			}
			memcpy(pp+local_index,v1.lcr,3);
			local_index+=3;
		}
		
	}
	return ret_address;
}
		


int main(int argc,char * argv[])
{
	node * root = node_gen();

	char arr[MAX_L];
	int i=0;
	int meanOfset=0;
	FILE * fin = fopen(argv[1],"r");
	FILE * fout = fopen(argv[2],"w+");
	if( fin != NULL && fout!=NULL )
	{
		while( (arr[i]=fgetc(fin)) != EOF   )
		{
			if( arr[i] == '\n' )
			{
				arr[i] = '\0';
				add_to_trie(arr,meanOfset,root);
				meanOfset+=i+1;
				//printf("%s\n",arr);
				arr[0] = '\0';
				i=0;
			}
			else
			{
				i++;
			}
		}
		rewind(fin);
		i=0;
		int flag = 1;

		while(((arr[i]=fgetc(fin)) != EOF) && flag!=-1)
		{
			if( arr[i] == '\n' )
			{
				arr[i] = '\0';
				flag=srch(arr,root);
				printf("%s is present %d\n",arr,flag);
				arr[0]='\0';
				i=0;
			}
			else
			{
				i++;
			}

		}
		if( flag==-1 ) printf("inconsitent insert\n");

		int *ar = malloc(4*(ALPHA_SIZE+1)*(ALPHA_SIZE+1));
		memset(ar,0,(ALPHA_SIZE+1)*(ALPHA_SIZE+1)*4);
		printf("count start\n");
		count_nodes(root,ar);
		int type_count[3];
		memset(type_count,0,12);
		printf("count complete\n");
		int total_node = 0;
		fprintf(fout,"     ");
		for( int i=0;i<18;i++) fprintf(fout,"%6d ",i);
		fprintf(fout,"\n");
		for(int i=0;i<=max_l;i++)
		{
			fprintf(fout,"%2d -",i);
			for(int j=0;j<=max_m;j++)
			{
				if( i==0 && j!=0 )
				{
					type_count[1]+=ar[i*ALPHA_SIZE + j];
				}
				else if( i!=0 && j==0 )
				{
					type_count[0]+=ar[i*ALPHA_SIZE + j];
				}
				else if( i!=0 && j!=0 )
				{	
					type_count[2]+=ar[i*ALPHA_SIZE + j];
				}
				fprintf(fout,"%6d ",ar[i*ALPHA_SIZE + j]);
				total_node+=ar[i*ALPHA_SIZE + j];
			}
			fprintf(fout,"\n");
		}
		printf("total node : %d \n type 1 : %d \n type 2: %d \n type 3: %d \n",total_node,type_count[0],type_count[1],type_count[2]);
		fclose(fout);
		fclose(fin);
		printf("generating flat\n");
		node * ap = node_gen();
		printf("generating flat\n");
		ap->nnode[1] = NULL;
		//ap->meaning = ap->meaning | 1<<3;
		printf("generating flat\n");
		int ret = flat_gen(root);
		printf("%d,%d\n",ret,global_index);




	}

}

int lin_search(char *word)
{
	int lcl_idx = 0;
	int flg=1;
	int retval = -1;
	union tp{ int i; char iin[3]; } v1,v2;
	while( *(word+1) != '\0' && flg!=0 )
	{
		if( pp[lcl_idx] != T2 )
		{
			lcl_idx+=1;
			if( pp[(lcl_idx+ (*(word) - 'a'))*4] != -1 )
			{
				lcl_idx = pp[(lcl_idx +(*word - 'a'))*4];
				word+=1;
			}
			else
			{
				flg=0;
			}
		}
		else
		{
			flg=0;
		}
	}
	if( flg != -1 && pp[lcl_idx] != T1 )
	{
		if( pp[lcl_idx] == T2 )
		{
			int j= *((int *) (pp+lcl_idx));
			j = (j<<8)>>8;
			if( j != ( (((int)-1)<<8)>>8 ) )
			{
				retval = j;
			}
		}
		else
		{
			int j= *((int *)(pp+lcl_index
	}


			

//for searching the word
/*
	start with first character.
		c1 look in root node if the that character exists in the root node.
		
		return type
		*/
