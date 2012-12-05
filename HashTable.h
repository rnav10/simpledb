#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

unsigned long cryptTable[0x500];
int isCryptTableCreated=0;

struct MPQHASHTABLE
{
    int nHashA;
    int nHashB;
    char bExists;
    void *value;
	int  isReadFlag;
	float  orderValue;
};

void prepareCryptTable()
{ 
    unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
    for( index1 = 0; index1 < 0x100; index1++ )
    { 
        for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 )
        { 
            unsigned long temp1, temp2;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);
            cryptTable[index2] = ( temp1 | temp2 ); 
       } 
   }
   isCryptTableCreated=1;
}

unsigned long HashString( char *lpszFileName, unsigned long dwHashType )
{ 
    unsigned char *key  = (unsigned char *)lpszFileName;
    unsigned long seed1 = 0x7FED7FED;
    unsigned long seed2 = 0xEEEEEEEE;
    int ch;
    while( *key != 0 )
    { 
        ch = toupper(*key++);
        seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
    }
    return seed1; 
}
 

unsigned long GetHashTablePos(char *lpszString,struct MPQHASHTABLE *lpTable, int nTableSize)
{
    unsigned long  nHash = HashString( lpszString,0 );
    unsigned long  nHashA = HashString( lpszString,1);
    unsigned long  nHashB = HashString( lpszString,2 );
    unsigned long  nHashStart = nHash % nTableSize;
    unsigned long  nHashPos = nHashStart;
    while ( lpTable[nHashPos].bExists )
    {
        if ( lpTable[nHashPos].nHashA == nHashA &&  lpTable[nHashPos].nHashB == nHashB )
		{
			return nHashPos;
		}
        else
		{
            nHashPos = (nHashPos + 1) % nTableSize;
		}
        if (nHashPos == nHashStart)break;     
    }
    return -1;
}


void intiHashTb(struct MPQHASHTABLE *hashtb,int start,int length)
{
	for(;start<length;start++)
	{
		hashtb[start].nHashA=-1;
		hashtb[start].nHashB=-1;
		hashtb[start].bExists=0;
		hashtb[start].orderValue=0;
	}
}

struct MPQHASHTABLE *newHashtable(int *tblen)
{
	struct MPQHASHTABLE *hashtb;
	hashtb=(struct MPQHASHTABLE *)malloc(1024*sizeof(struct MPQHASHTABLE));
	*tblen=1024;
	intiHashTb(hashtb,0,1024);
	if(!isCryptTableCreated)prepareCryptTable();
	return hashtb;
}

int addKeyValue(struct MPQHASHTABLE *hashtb,char *key,void *value,int *length)
{
	unsigned long ulHashValue;
	ulHashValue = HashString(key,0);
	ulHashValue= ulHashValue % *length;
	if(ulHashValue>=*length)
	{
		realloc(hashtb,2*(*length)*sizeof(struct MPQHASHTABLE));
		intiHashTb(hashtb,*length,(*length)*2);
		*length=(*length)*2;
	}
	hashtb[ulHashValue].nHashA=HashString(key,1);
	hashtb[ulHashValue].nHashB=HashString(key,2);
	hashtb[ulHashValue].bExists=1;
	hashtb[ulHashValue].isReadFlag=1;
	hashtb[ulHashValue].value=value;
	return ulHashValue;
}

void disposeHashTable(struct MPQHASHTABLE *hashtb)
{
	free(hashtb);
}

