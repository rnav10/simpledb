#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"RM_definition.h"
#include"PF_definition.h"

int tableBlock=5;
int colomeBlock=10;
int recordBlock=30;

struct database *first;
int dbcounts;
int currentSize;
struct tableBuffer tbuffer;
struct dbBuffer dbbuffer;

void intiStorage()
{
	first=(struct database *)malloc(5*sizeof(struct database));
	if(first == NULL)
	{
		printf("memory fails");
		return;
	}
	dbcounts=0;
	currentSize=10;
	tbuffer.pointer=NULL;
	dbbuffer.pointer=NULL;
}

struct table *getTableBuffer(char *tbname,char *dbname)
{
	if(tbuffer.pointer==NULL)return NULL;
	if(!strcmp(dbname,tbuffer.dbname) && !strcmp(tbname,tbuffer.tbname))return tbuffer.pointer;
	else return NULL;
}

struct database *getDbBuffer(char *dbname)
{
	if(tbuffer.pointer==NULL)return NULL;
	if(!strcmp(dbname,tbuffer.dbname))return dbbuffer.pointer;
	else return NULL;
}

struct database *getDatabase(char *name)
{
	 struct database *pdb=first;
	 int i;
	 if(dbcounts ==0)return NULL;
	 for(i=0;i<dbcounts;i++)
	 {
		 if((pdb+i)->isDelete)continue;
		 if(!strcmp((pdb+i)->dbname,name))
		 {
			 strcpy(dbbuffer.dbname,name);
			 dbbuffer.pointer=pdb+i;
			 return pdb+i;
		 }
	 }
	 return NULL;
}

void NewDb(char *dbName)
{
	FILE *fp;
	if(dbcounts>=currentSize)
	{
		first=(struct database *)realloc(first,(currentSize+5)*sizeof(struct database));
		if(first != NULL)currentSize+=5;
		else
		{
			printf("memeory fail!");
			return;
		}
	}
	strcpy((first+dbcounts)->dbname,dbName);
	(first+dbcounts)->firstTable=NULL;
	(first+dbcounts)->isDelete=0;
	(first+dbcounts)->tableCapacity=0;
	(first+dbcounts)->tableCounts=0;
	(first+dbcounts)->colomeCapacity=0;
	(first+dbcounts)->colomeCounts=0;
	(first+dbcounts)->colomes=NULL;
	dbcounts++;
	if((fp=fopen(dbName,"w")))return;
	fwrite(first+dbcounts-1,sizeof(struct database),1,fp);
	fclose(fp);
}

struct table *newTable(char *dbname)
{
	struct database *db=NULL;
	struct table *tb=NULL;
	db=getDatabase(dbname);
	if(db==NULL)
	{
		return NULL;
	}
	if(db->firstTable == NULL)
	{
		db->firstTable=(struct table *)malloc(sizeof(struct table)*tableBlock);
		db->tableCapacity=tableBlock;
	}
	if(db->tableCounts==db->tableCapacity)
	{
		realloc(db->firstTable,sizeof(struct table)*(db->tableCapacity+tableBlock));
		db->tableCapacity+=tableBlock;
	}
	tb=db->firstTable+db->tableCounts;
	db->tableCounts++;
	return tb;
}

void removeDb(char *name)
{
	 struct database *pdb=first;
	 int i=0;
	 if(dbcounts ==0)return ;
	 for(i=0;i<dbcounts;i++)
	 {
		 if((pdb+i)->isDelete)continue;
		 if(!strcmp((pdb+i)->dbname,name))
		 {
			 (pdb+i)->isDelete=1;
			 if((pdb+i)->firstTable!=NULL)free((pdb+i)->firstTable);
			 return;
		 }
	 }
}

struct table *getTableFromDatabase(char *dbname,char *tbname)
{
	struct database *db;
	struct table *ptb;
	int i;
	db=getDatabase(dbname);
	if(db==NULL)
	{
		printf("database not found");
		return NULL; 
	}
	ptb=db->firstTable;
	if(ptb==NULL)return NULL;
	for(i=0;i<db->tableCounts;i++)
	{
		 if((ptb+i)->isDelete)continue;
		 if(!strcmp((ptb+i)->name,tbname))
		 {
			 strcpy(tbuffer.tbname,tbname);
			 strcpy(tbuffer.dbname,dbname);
			 tbuffer.pointer=ptb+i;
			 return ptb+i;
		 }
	}
	return NULL;
}

struct table *getTableFromBuffer(char *dbname,char *tbname)
{
	 struct table *tb=NULL;
	 tb=getTableBuffer(tbname,dbname);
	 if(tb==NULL)tb=getTableFromDatabase(dbname,tbname);
	 return tb;
}

struct colome *newColome(char *dbname,char *tbname,int cacheFlag)
{
	struct table *tb=NULL;
	struct colome *col=NULL;
	struct database *db=NULL;
	if(cacheFlag)
	{
		db=getDbBuffer(dbname);
		tb=getTableBuffer(tbname,dbname);
		if(db==NULL)db=getDatabase(dbname);
		if(tb==NULL)tb=getTableFromDatabase(dbname,tbname);
	}
	else
	{
		db=getDatabase(dbname);
	    tb=getTableFromDatabase(dbname,tbname);
	}
	if(tb==NULL)
	{
		return NULL;
	}
	if(db->colomes == NULL)
	{
		db->colomes=(struct colome *)malloc(sizeof(struct colome)*colomeBlock);
		db->colomeCapacity=colomeBlock;
	}
	if(db->colomeCounts==db->colomeCapacity)
	{
		realloc(db->colomes,sizeof(struct colome)*(db->tableCapacity+colomeBlock));
		db->colomeCapacity+=colomeBlock;
	}
	col=db->colomes+db->colomeCounts;
	db->colomeCounts++;
	if(tb->firstColome == NULL)tb->firstColome=col;
	tb->colomeCounts++;
	//col->colomeID=tb->colomeCounts;
	return col;
}

int recordSizeCount(struct colome *head,int count)
{
	int size=sizeof(int);
	int i;
	for( i=0;i<count;i++)
	{
		switch((head+i)->type)
		{
		   case VARCHAR:size+=sizeof(char)*(head+i)->strLength;break;
		   case INT:size+=sizeof(int);break;
		   case FLOAT:size+=sizeof(float);break;
		   default:;
		}
	}
	return size;
}

void *newRecord(char *dbname,char *tbname)
{
	struct table *tb;
	void *record;
	int recordSize;
	tb=getTableFromDatabase(dbname,tbname);
	if(tb==NULL)
	{
		return NULL;
	}
	if(tb->firstColome == NULL)
	{
		return NULL;
	}
	recordSize=recordSizeCount(tb->firstColome,tb->colomeCounts);
	if(tb->firstRecord== NULL)
	{
		tb->firstRecord=(struct colome *)malloc(recordSize*recordBlock);
		tb->recordCapacity=recordBlock;
	}
	if(tb->recordCounts==tb->recordCapacity)
	{
		realloc(tb->firstColome,recordSize*(tb->recordCapacity+recordBlock));
		tb->recordCapacity+=tableBlock;
	}
    if(tb->recordCounts == 0)record=tb->firstRecord;
	else 
	{ 
		record=(char *)tb->firstRecord+tb->recordCounts*recordSize;
	}
	tb->recordCounts=tb->recordCounts+1;
	return record;
}

void loadDataBase(char *dbname)
{
	/*FILE *fp;
	if((fp=fopen(dbname,"r")))return;
	if(dbcounts>=currentSize)
	{
		first=(struct database *)realloc(first,(currentSize+5)*sizeof(struct database));
		if(first != NULL)currentSize+=5;
		else
		{
			printf("memeory fail!");
			return;
		}
	}
	fread(first+dbcounts,sizeof(struct database),1,fp);
	fclose(fp);*/
}

/*
void getRecordFromTable(char *dbname,char *tbname,int recordId,struct record *precord)
{

}*/
