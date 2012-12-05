#include<string.h>
#include"SM_Manage.h"
#include"QL_Manage.h"

char *dbname=NULL;
void TrimAllStr(char *ibuf);
void verbCreate(char *input);
void verbUse(char *input);
void verbDrop(char *input);
void verbInsert(char *input);


void CommandParaser(char *input)
{
	char actionVerb[10]={0},*p;
	memccpy(actionVerb,input,' ',7);
	p=strchr(actionVerb,0);
	if(p==NULL)return;//报错
	else *(p-1)='\0';
	if(!strcmp(actionVerb,"create"))
	{
		verbCreate(input);
		return;
	}
	else if(!strcmp(actionVerb,"drop"))
	{
		verbDrop(input+5);
		return;
	}
	else if(!strcmp(actionVerb,"use"))
	{
		verbUse(input+4);
		return;
	}
	//else if(!strcmp(actionVerb,"load"))
	else if(!strcmp(actionVerb,"select"))
	{
		if(dbname==NULL)return;
		//(input+7);
		return;
	}
	else if(!strcmp(actionVerb,"insert"))
	{
		if(dbname==NULL)return;
		verbInsert(input+7);
		return;
	}
	//else if(!strcmp(actionVerb,"update"))
	//else if(!strcmp(actionVerb,"delete"))
	else return ;//报错*/
}

void verbCreate(char *input)
{
	char object[15]={0},*p;
	memccpy(object,input+7,' ',10);
	p=strchr(object,0);
	if(p==NULL)return;
	else *(p-1)='\0';
	if(!strcmp(object,"database"))
	{
		char para[20]={0};
		memccpy(para,input+7+9,'\0',20);
		createDb(para);
		return ;
	}
	else if(!strcmp(object,"table"))
	{
		char para[200]={0},tbname[20]={0},*pstart,*pend;
		int length=0,capacity=0;
		void *ret;
	    struct attrInfo *attrs;
		attrs=(struct attrInfo *)malloc(5*sizeof(struct attrInfo));
		capacity=5;
		if(dbname==NULL)return;//报错
		memccpy(para,input+7+7,' ',200);
		memccpy(tbname,input+7+7,'{',20);
		pstart=strchr(para,'{');
		pend=strchr(para,'}');
		pstart++;
		pend--;
		while(pstart != pend)
	    {
			char item[50]={0},*pSeperate;
			if(length==capacity)
			{
				realloc(attrs,sizeof(struct attrInfo)*(capacity+5));
				capacity+=5;
			}
			memccpy(item,pstart,',',50);
			ret=memccpy(attrs->colname,item,' ',20);
			pSeperate=strchr(item,' ');
			////////////////////////解析列数据类型;
			while(*pstart != ',' && *pstart!='\0')pstart++;
			if(*pstart==',')pstart++;
			if(ret != NULL)
			{
				*((char *)ret-1)='\0';
			}

			length++;
			if(length!=capacity)attrs++;
	    }
		attrs=attrs-length+1;
		createTable(dbname,tbname,attrs,length);
	}
}

void verbUse(char *input)
{
	if(isDbExiste(input))
	{
        if(dbname!=NULL)free(dbname);
		dbname=(char *)malloc(20);
		strcpy(dbname,input);
		printf("database has changed\n");
	}
	else
	{
		printf("unkown database\n");
	}
}

void verbDrop(char *input)
{
	char object[15]={0},*p;
	memccpy(object,input,' ',10);
	p=strchr(object,0);
	if(p==NULL)return;
	else *(p-1)='\0';
	if(!strcmp(object,"database"))
	{
		char para[20]={0};
		memccpy(para,input+9,'\0',20);
		deleteDataBase(para);
		printf("Query Ok");
		return;
	}
	else if(!strcmp(object,"table"))
	{
		char para[20]={0};
		if(dbname==NULL)return;//报错
		memccpy(para,input+6,'\0',20);
		deleteTable(dbname,para);
	}
	else
	{
		return;
	}
}

void verbInsert(char *input)
{
	char *pstart,*pend,tbname[20]={0};
	int length=0;
	int capacity=0;
	struct value *vlist=NULL,*listhead=NULL;
	void *ret=NULL;
	ret=memccpy(tbname,input+5,'(',20);
	if(ret != NULL)
	{
		*((char *)ret-1)='\0';
	}
	pstart=strchr(input,'(');
	pend=strchr(input,')');
	pstart++;
	vlist=(struct value *)malloc(30*sizeof(struct value));
	listhead=vlist;
	capacity=20;
	while(pstart!=pend)
	{
		if(length==capacity)
		{
			realloc(listhead,sizeof(struct value)*(capacity+5));
			capacity+=5;
			//严重bug
		}
		memset(vlist->values,0,100);
        ret=memccpy(vlist->values,pstart,',',20);
		TrimAllStr(vlist->values);
		while(*pstart != ',' && *pstart!=')')pstart++;
		if(*pstart==',')pstart++;
		if(ret != NULL)
		{
			*((char *)ret-1)='\0';
		}
		length++;
		if(length!=capacity)vlist++;
	}
	InsertRecord(listhead,length,tbname,dbname);
}

void TrimAllStr(char *ibuf)
{
        int nlen,i;
        /*判断字符指针是否为空及字符长度是否为0*/
        if (ibuf == NULL) 
                return;
        nlen = strlen(ibuf);
        if (nlen == 0)
                return;
        /*去前空格*/
        i = 0;
        while (1)
        {
                if (ibuf[i]!=' ')
                        break;
                i ++;
        }
        if (i == nlen)
        {
                ibuf[0] = 0;
                return;
        }
        if (i != 0)
        {
                nlen = nlen - i + 1;
                memcpy(ibuf,&ibuf[i],nlen);
        }
        /*去后空格*/
        nlen = strlen(ibuf) - 1;
        while (nlen>=0)
        {
                if (ibuf[nlen] != ' ')
                {
                        ibuf[nlen + 1] = 0;
                        break;
                }
                nlen --;
        }
}

void verbSelect(char *input)
{
	char *pSelectClauseStart=NULL,*pSelectClauseEnd=NULL,*pWhereClauseStart=NULL;
	char *pOrderClauseStart=NULL,*pWhereClauseEnd=NULL;
	char selectClause[100],whereClause[100]={0},orderClause={100};
	pSelectClauseEnd=strstr(input,"from");
	pSelectClauseStart=input;
    pWhereClauseStart=strstr(input,"where");
	pWhereClauseEnd=strstr(input,"order");
	pOrderClauseStart=pWhereClauseEnd;
}

struct Condition *expleinWhereClause(char *pwhere,int *length)
{
	char *p,*pand,*pstart,item[50]={0};
	int len=0,oplen=1;
	char *ret=NULL;
	struct Condition *conditions,*head;
	enum Operator type; 
	conditions=(struct Condition *)malloc(20*sizeof(struct Condition));
	if(conditions==NULL)return;//报错
	head=conditions;
	pstart=pwhere;
	while((pand=strstr(pstart,"and"))!=NULL)
	{
		memset(item,0,50);
		memcpy(item,pstart,pand-pstart);
		printf("%s\n",item);
		pstart=pand+4;
		oplen=1;
		if((p=strstr(item,"<"))!=NULL)
		{
			if((p+1)!=0 && *(p+1)=='=')
			{
				p++;
				oplen=2;
				type=LE;
			}
			else if((p+1)!=0 && *(p+1)=='>')
			{
				p++;
				oplen=2;
				type=NE;
			}
			else
			{
				type=LT;
			}
		}
		if((p=strstr(item,">"))!=NULL)
		{
			if((p+1)!=0 && *(p+1)=='=')
			{
				p++;
				oplen=2;
				type=GE;
			}
			else
			{
				type=GT;
			}
		}
		else if((p=strstr(item,"="))!=NULL)type=EQ;
		else continue;
		memset(conditions->attrname,0,20);
		memcpy(conditions->attrname,item,p-item-oplen+1);
		memset(conditions->rhsValue,0,100);
		memccpy(conditions->rhsValue,p+1,'\0',20);
		TrimAllStr(conditions->rhsValue);
		conditions->op=type;
		conditions++;
		len++;
	}
	memset(item,0,50);
	memccpy(item,pstart,'\0',20);
	printf("%s\n",item);
	oplen=1;
	if((p=strstr(item,"<"))!=NULL)
	{
		if((p+1)!=0 && *(p+1)=='=')
		{
			p++;
			oplen=2;
			type=LE;
		}
		else if((p+1)!=0 && *(p+1)=='>')
		{
			p++;
			oplen=2;
            type=NE;
		}
		else
		{
            type=LT;
		}
	}
	else if((p=strstr(item,">"))!=NULL)
	{
		if((p+1)!=0 && *(p+1)=='=')
		{
			p++;
			oplen=2;
			type=GE;
		}
		else
		{
            type=GT;
		}
	}
	else if((p=strstr(item,"="))!=NULL)type=EQ;
	else item[0]='\0';
	if(item[0]!='\0')
	{
		memset(conditions->attrname,0,20);
		memcpy(conditions->attrname,item,p-item-oplen+1);
		memset(conditions->rhsValue,0,100);
		memccpy(conditions->rhsValue,p+1,'\0',20);
		TrimAllStr(conditions->rhsValue);
		conditions->op=type;
		conditions++;
		len++;
	}
	//*length=len;
	return head;
}



