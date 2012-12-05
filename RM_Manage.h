#include<stdio.h>
#include<string.h>
#include"RM_lib.h"
#include"PF_Manage.h"
#include<stdlib.h>

void *nextRowValue(void *record,struct colome *col)
{
	if(record ==NULL || col==NULL)
	{
		return;
	}
	else
	{
		switch(col->type)
		{
		   case VARCHAR:record=(char *)record+col->strLength;;break;
		   case INT:record=(int *)record+1;break;
		   case FLOAT:record=(float *)record+1;break;
		   default:;
		}
	}
	return record;
}

void *InsertRecord(struct value *vals,int length,char *tbname,char *dbname)
{
	struct table *tb;
	void *rowVal,*precord;
	int colomeSize,i;
	struct record *currentRecord,*tp;
	struct colome *tcol,*tpcol,*pcol;
	tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)
		return NULL;
	rowVal=newRecord(dbname,tbname);
    if(rowVal==NULL)return NULL;
	if(length != tb->colomeCounts)return NULL;
	pcol=tpcol=tb->firstColome;
	*((int *)(rowVal))=1;
	precord=rowVal;
	rowVal=(int *)rowVal+1;
	for(i=0;i<length;i++)
	{
		tpcol=pcol+i;
		switch(tpcol->type)
		{
			case VARCHAR:memcpy(rowVal,(vals+i)->values,tpcol->strLength);rowVal=(void *)((char *)rowVal+tpcol->strLength);break;
			case INT:*((int *)rowVal)=atoi((vals+i)->values);rowVal=((int *)rowVal+1);break;
			case FLOAT:*((float *)rowVal)=atof((vals+i)->values);rowVal=((float *)rowVal+1);break;
			default:;
		}
	}
	return precord;
}

void deleteRecord(void *record)
{
	if(record ==NULL)return;
	*((int *)record)=0; 
}

void emptyTable(char *dbname,char *tbname)
{
	struct table *tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)return;
	free(tb->firstRecord);
	tb->firstRecord=NULL;
	tb->recordCapacity=0;
	tb->recordCounts=0;
}

struct iterator
{
	char *pdata;
	enum Operator op;
	char *colname;
	void *strat;
	struct colome *col;
	int colLength;
	int recordCount;
	int recordsize;
	int isNULLConditon;
	int position;
};

struct iterator *searchRecord(char *dbname,char *tbname,enum Operator op,char *colname,char *pdata,int isNULLConditon)
{
	struct table *tb;
	struct iterator *ite;
	ite=(struct iterator *)malloc(sizeof(struct iterator));
	tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)
	{
		RM_printfError("table not found");
		return NULL;
	}
	ite->colname=colname;
	ite->op=op;
	ite->pdata=pdata;
	ite->strat=tb->firstRecord;
	ite->col=tb->firstColome;
	ite->colLength=tb->colomeCounts;
	ite->recordCount=tb->recordCounts;
	ite->recordsize=recordSizeCount(tb->firstColome,tb->colomeCounts);
	ite->isNULLConditon=isNULLConditon;
	ite->position=0;
	return ite;
}

void closeIterator(struct iterator *lte)
{
	free(lte);
}

void *nextRecord(struct iterator *ite)
{
	int isMatch=0;
    struct colome *col,*tp;
	void *rowVal,*record;
	int i;
	rowVal=ite->strat;
	while(rowVal!=NULL && ite->position<ite->recordCount)
	{
		record=rowVal;
		if(*((int *)rowVal)==0)
		{
			ite->position=ite->position+1;
	        ite->strat=(char *)record+ite->recordsize;
			rowVal=ite->strat;
			continue;
		}
		rowVal=(int *)rowVal+1;
		if(ite->isNULLConditon)
		{
			ite->position=ite->position+1;
	        ite->strat=(char *)record+ite->recordsize;
			return record;
		}
		isMatch=0;
		for(i=0;i<ite->colLength;i++)
		{
			col=ite->col+i;
			if(!strcmp(col->name,ite->colname))
			{
				switch(col->type)
				{
					case VARCHAR:isMatch=varcharOperate(ite->pdata,rowVal,col->strLength,&(ite->op));break;
					case INT:isMatch=intOperate(ite->pdata,rowVal,&(ite->op));break;
					case FLOAT:isMatch=floatOperate(ite->pdata,rowVal,&(ite->op));break;
					default:;
				}
				if(isMatch)
				{
					ite->strat=(char *)record+ite->recordsize;
					ite->position=ite->position+1;
					return record;
				}
			}
			else
			{
				rowVal=nextRowValue(rowVal,col);
			}
	   }
	   ite->position=ite->position+1;
	   ite->strat=(char *)record+ite->recordsize;
	   rowVal=ite->strat;
	}
	return NULL;
}

void getData(void *pvalue,char *dbname,char *tbname,char *value)
{
	struct table *tb;
	struct colome *col;
	void *rowVal;
	char num[100],*output;
	int i;
	tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)
	{
		RM_printfError("table not found");
		return;
	}
	rowVal=pvalue;
	rowVal=(int *)rowVal+1;
	output=value;
	for(i=0;i<tb->colomeCounts;i++)
	{
		col=tb->firstColome+i;
		switch(col->type)
		{
			case VARCHAR:memccpy(output,(char *)rowVal,'\0',col->strLength);
				 output=output+strlen((char *)rowVal);
				 rowVal=(char *)rowVal+col->strLength;
				 break;
			case INT:sprintf(num,"%d",*(int *)rowVal);
				 memccpy(output,num,'\0',10);
				 rowVal=(int *)rowVal+1;
				 output=(char *)output+strlen(num);
				 break;
			case FLOAT:sprintf(num,"%f",*(float *)rowVal);
				  memccpy(output,num,'\0',10);
				  rowVal=(float *)rowVal+1;
				  output=(char *)output+strlen(num);
				  break;
			default:;
		}
		*output='|';
		output=(char *)output+1;
	}
	//output=(char *)output+1;
	*output='\0';
}

int varcharOperate(char *pdata,void *rowVal,int length,enum Operator *op)
{
	
	int ret=1;
	if(*op==EQ)
	{
		if(!strcmp((char *)rowVal,pdata))return 1;
		else return 0;
	}
	else if(*op==NE)
	{
		if(!strcmp((char *)rowVal,pdata))return 0;
		else return 1;
	}
	else
	{
		return 0;
	}
}

int intOperate(char *pdata,void *rowVal,enum Operator *op)
{
	int *row=(int *)rowVal;
	int data=atoi(pdata);
	if(*op==EQ)
	{
		if(*row==data)return 1;
		else return 0;
	}
	else if(*op==NE)
	{
		if(*row!=data)return 1;
		else return 0;
	}
	else if(*op==NE)
	{
		if(*row!=data)return 1;
		else return 0;
	}
	else if(*op==LT)
	{
		if(*row<data)return 1;
		else return 0;
	}
	else if(*op==GT)
	{
		if(*row>data)return 1;
		else return 0;
	}
	else if(*op==LE)
	{
		if(*row<=data)return 1;
		else return 0;
	}
	else if(*op==GE)
	{
		if(*row>=data)return 1;
		else return 0;
	}
	else
	{
		return 0;
	}
}

int floatOperate(char *pdata,void *rowVal,enum Operator *op)
{
	float *row=(float *)rowVal;
	float data=atof(pdata);
	if(*op==EQ)
	{
		if(*row==data)return 1;
		else return 0;
	}
	else if(*op==NE)
	{
		if(*row!=data)return 1;
		else return 0;
	}
	else if(*op==NE)
	{
		if(*row!=data)return 1;
		else return 0;
	}
	else if(*op==LT)
	{
		if(*row<data)return 1;
		else return 0;
	}
	else if(*op==GT)
	{
		if(*row>data)return 1;
		else return 0;
	}
	else if(*op==LE)
	{
		if(*row<=data)return 1;
		else return 0;
	}
	else if(*op==GE)
	{
		if(*row>=data)return 1;
		else return 0;
	}
	else
	{
		return 0;
	}
}

void insertDataBase(char *dbName)
{
	
	NewDb(dbName);
}

void deleteDataBase(char *dbname)
{
	removeDb(dbname);
}

int insertTable(char *tbname,char *dbName)
{
	struct table *tb=NULL,*lastTb,*tp;
	tb=newTable(dbName);
	if(tb==NULL)
	{
		RM_printfError("database not found");
		return 0;
	}
	tb->firstColome=NULL;
	tb->firstRecord=NULL;
	strcpy(tb->name,tbname);
	tb->isDelete=0;
	tb->colomeCounts=0;
	tb->recordCapacity=0;
	tb->recordCounts=0;
	return 1;
}

void deleteTable(char *dbname,char *tbname)
{
	struct table *currentTb,*previous;
	currentTb=getTableFromDatabase(dbname,tbname);
	if(currentTb==NULL)
	{
		RM_printfError("table not found");
		return;
	}
    currentTb->isDelete=1;
	if(currentTb->firstColome != NULL)free(currentTb->firstColome);
	if(currentTb->firstRecord != NULL)free(currentTb->firstRecord);
}

void createColome(char *dbname,char *tbname,char *name,enum DataType type,int strLength,int cacheFlag)
{
	struct colome *newcol;
	newcol=newColome(dbname,tbname,cacheFlag);
	strcpy(newcol->name,name);
	newcol->type=type;
	newcol->strLength=strLength;
}

struct dataColome getDataColome(char *dbname,char *tbname,char *colname,void *value,int cacheFlag)
{
	struct table *tb;
	struct colome *col;
	struct dataColome dc;
	int i;
	dc.value=NULL;
	if(cacheFlag)tb=getTableFromBuffer(dbname,tbname);
	else tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)return dc;
	value=(int *)value+1;
	for(i=0;i<tb->colomeCounts;i++)
	{
		col=tb->firstColome+i;
		if(!strcmp(col->name,colname))
		{
			dc.value=value;
			dc.type=col->type;
			dc.strLength=col->strLength;
			return dc;
		}
		else
		{
			value=nextRowValue(value,col);
		}
	}
	return dc;
}

void updateRecord(char *dbname,char *tbname,char *colname,void *value,char *pdata)
{
	 struct dataColome dc=getDataColome(dbname,tbname,colname,value,1);
	 if(dc.value !=NULL && dc.type==INT )
		 *((int *)dc.value)=atoi(pdata);
     else if(dc.value !=NULL && dc.type==FLOAT)
	     *((float *)dc.value)=atoi(pdata);
	 else if(dc.value !=NULL && dc.type==VARCHAR)	
	     memcpy(dc.value,pdata,dc.strLength);
	 else ;	   			 
}

int isDbRecordExist(char *dbname)
{
	if(getDatabase(dbname)==NULL)
	{
		loadDataBase(dbname);
		if(getDatabase(dbname)==NULL)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else return 1;
}
