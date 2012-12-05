#include<stdlib.h>
#include<string.h>
#include"HashTable.h"

int paraBlock=5;

void printRecord(char *str)
{

}

void printHead(char *dbname,char *tbname)
{

}

void printFoot(char *str)
{

}

int cmp(const void *a,const void *b)
{
	return ((struct MPQHASHTABLE *)a)->orderValue-((struct MPQHASHTABLE *)b)->orderValue;
}

int cmpDESC(const void *a,const void *b)
{
	return ((struct MPQHASHTABLE *)b)->orderValue-((struct MPQHASHTABLE *)a)->orderValue;
}


void Insert(char *dbname,char *tbname,struct value *vlist,int length)
{
	void *ret;
	ret=InsertRecord(vlist,length,tbname,dbname);
	//getData(ret,dbname,tbname,ret1);
	//printHead();
	//printRecord();
	//printFoot();
	//printf("\n\n%s-----%d\n",ret1,ret);
	if(vlist !=NULL)free(vlist);
}

void Delete(char *dbname,char *tbname,struct Condition *conditions,int nConditons)
{
	void *record;
	int i,tblength,*plen,rowcount;
	struct MPQHASHTABLE *hashtb,*ptb;
	plen=&tblength;
	hashtb=newHashtable(plen);
	if(hashtb==NULL)
		return;
	for(i=0;i<nConditons;i++)
	{
		struct iterator *ite;
		if(conditions==NULL)
		{
			ite=searchRecord(dbname,tbname,EQ,NULL,NULL,1);
		}
		else
		{
			ite=searchRecord(dbname,tbname,(conditions+i)->op,(conditions+i)->attrname,(conditions+i)->rhsValue,0);
		}
		if(ite==NULL)
			break;
		rowcount=0;
		while((record=nextRecord(ite)) != NULL)
		{
			char ret[15];
			sprintf(ret,"%d",record);
			if(i<=0)
			{
			   int pos;
			   pos=addKeyValue(hashtb,ret,record,plen);
			}
			else
			{
				int pos;
				if((pos=GetHashTablePos(ret,hashtb,tblength))!=-1)
				{
					hashtb[pos].isReadFlag++;
				}
				pos=2;
			}	
		}
		closeIterator(ite);
	}

	for(i=0;i<tblength;i++)
	{
		if(hashtb[i].bExists != 1)
			continue;
		if(hashtb[i].isReadFlag==nConditons)
		{
			deleteRecord(hashtb[i].value);
		}
	}
	disposeHashTable(hashtb);
}

void Select(char *dbname,char *tbname,int nselectAttr,char *selectAttr,struct Condition *conditions,int nConditons,int order,struct relAttr orderAttr)
{
	void *record;
	int i,IsHeadPrinted,tblength,*plen,rowcount;
	struct MPQHASHTABLE *hashtb,*ptb;
	plen=&tblength;
	hashtb=newHashtable(plen);
	if(hashtb==NULL)
		return;
	for(i=0;i<nConditons;i++)
	{
		struct iterator *ite;
		if(conditions==NULL)
		{
			ite=searchRecord(dbname,tbname,EQ,NULL,NULL,1);
		}
		else
		{
			ite=searchRecord(dbname,tbname,(conditions+i)->op,(conditions+i)->attrname,(conditions+i)->rhsValue,0);
		}
		if(ite==NULL)
			break;
		rowcount=0;
		while((record=nextRecord(ite)) != NULL)
		{
			char ret[15];
			sprintf(ret,"%d",record);
			if(i<=0)
			{
			   int pos;
			   pos=addKeyValue(hashtb,ret,record,plen);
			   
			   if(order==0)hashtb[pos].orderValue=rowcount;//可能需要检查order列类型
			   else 
			   {
                   struct dataColome dc;
				   dc=getDataColome(dbname,tbname,orderAttr.attrname,record,1);
				   if(dc.value !=NULL && dc.type==INT )
                     hashtb[pos].orderValue=*((int *)dc.value);
				   else if(dc.value !=NULL && dc.type==FLOAT)
					 hashtb[pos].orderValue=*((float *)dc.value);
				   else 
					 hashtb[pos].orderValue=rowcount;
				   
			   }
			   rowcount++;
			   printf("%d\n",record);
			}
			else
			{
				int pos;
				if((pos=GetHashTablePos(ret,hashtb,tblength))!=-1)
				{
					hashtb[pos].isReadFlag++;
				}
				pos=2;
			}	
		}
		closeIterator(ite);
	}
	if(order==-1)qsort(hashtb,tblength,sizeof(struct MPQHASHTABLE),cmpDESC);
	else qsort(hashtb,tblength,sizeof(struct MPQHASHTABLE),cmp);
	
	
	for(i=0;i<tblength;i++)
	{
		if(hashtb[i].bExists != 1)
			continue;
		if(hashtb[i].isReadFlag==nConditons)
		{
			char ret[100];
			getData(hashtb[i].value,dbname,tbname,ret);
			printf("%d-----------%s\n",hashtb[i].value,ret);
		}
	}

	disposeHashTable(hashtb);
		//if(IsHeadPrinted)printFoot();
		//sprintf(ret,"%")
			//addKeyValue(hashtb,)
			/*if(!IsHeadPrinted)
			{
				printHead(dbname,tbname);
				IsHeadPrinted=1;
			}*/
			//getData(record,dbname,tbname,ret);
	
}

void Update(char *dbname,char *tbname,struct Condition *conditions,int nConditons,char *attrName,char *pdata)
{
	void *record;
	int i,IsHeadPrinted,tblength,*plen,rowcount;
	struct MPQHASHTABLE *hashtb,*ptb;
	plen=&tblength;
	hashtb=newHashtable(plen);
	if(hashtb==NULL)
		return;
	for(i=0;i<nConditons;i++)
	{
		struct iterator *ite;
		if(conditions==NULL)
		{
			ite=searchRecord(dbname,tbname,EQ,NULL,NULL,1);
		}
		else
		{
			ite=searchRecord(dbname,tbname,(conditions+i)->op,(conditions+i)->attrname,(conditions+i)->rhsValue,0);
		}
		if(ite==NULL)
			break;
		rowcount=0;
		while((record=nextRecord(ite)) != NULL)
		{
			char ret[15];
			sprintf(ret,"%d",record);
			if(i<=0)
			{
			   int pos;
			   pos=addKeyValue(hashtb,ret,record,plen);
			   rowcount++;
			   printf("%d\n",record);
			}
			else
			{
				int pos;
				if((pos=GetHashTablePos(ret,hashtb,tblength))!=-1)
				{
					hashtb[pos].isReadFlag++;
				}
				pos=2;
			}	
		}
		closeIterator(ite);
	}
	for(i=0;i<tblength;i++)
	{
		if(hashtb[i].bExists != 1)
			continue;
		if(hashtb[i].isReadFlag==nConditons)
		{
			updateRecord(dbname,tbname,attrName,hashtb[i].value,pdata);
		}
	}

	disposeHashTable(hashtb);
}

