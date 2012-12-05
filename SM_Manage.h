#include"RM_Manage.h"

struct attrInfo
{
	char colname[20];
	enum DataType type;
	int  charLength;
};

void createDb(char *dbname)
{
	if(isDbExiste(dbname))return;
	if(strlen(dbname)>=20)
	{
		printf("name too long");
		return;
	}
	insertDataBase(dbname);
}

void dropDb(char *dbname)
{
	deleteDataBase(dbname);
}
void createTable(char *dbname,char *tbname,struct attrInfo *pattr,int attrlength)
{
	int i;
	if(pattr ==NULL)
	{
		printf("no colomes");
		return;
	}
   	if(!insertTable(tbname,dbname))return;
	for(i=0;i<attrlength;i++)
	{
		if((pattr+i)==NULL)break;
		createColome(dbname,tbname,(pattr+i)->colname,(pattr+i)->type,(pattr+i)->charLength,1);
	}
}
void dropTable(char *dbname,char *tbname)
{
	deleteTable(dbname,tbname);
}

int isDbExiste(char *dbname)
{
	return isDbRecordExist(dbname);
}



