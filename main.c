#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"Paraser.h"
void main()
{
       /* int i=0;
	   struct attrInfo attrs[5];
	   struct Condition cond[3];
	   struct relAttr yu;
	   intiStorage();
       printf("\tSimpleDB v1.00\n");
       
	   /*char s[16],ch;
       while((ch=getchar())!='\n')
       {
		   s[i++]=ch;
       }
	   s[i]='\0';*/

/*	   createDb("db1");
	   createTable("db1","test",NULL,0);
	   createTable("db1","test2",NULL,0);
	   dropTable("db1","test");
	   dropTable("db1","test");
	   dropDb("db1");
	   createTable("db1","test",NULL,0);
	   createDb("db2");
	   
	   attrs[0].type=VARCHAR;
	   strcpy(attrs[0].colname,"name");
	   attrs[0].charLength=20;
	    strcpy(attrs[1].colname,"eng");
	   attrs[1].type=INT;
	    strcpy(attrs[2].colname,"math");
	   attrs[2].type=INT;
	    strcpy(attrs[3].colname,"info");
	   attrs[3].type=INT;
	    strcpy(attrs[4].colname,"avg");
	   attrs[4].type=FLOAT;
	   createTable("db2","students",attrs,5);
	   createTable("db2","stubak",attrs,5);
	   createTable("db2","stutyu",attrs,5);
	   Insert("db2","stubak","dyiheng,43,32,543,89.32");
	   Insert("db2","students","dyiheng,43,32,543,87.32");
	   Insert("db2","students","dyiheng,56,35,543,87.32");
	   Insert("db2","students","dyiheng,78,47,543,87.32");
	   Insert("db2","students","neo,100,90,543,87.32");
	   Insert("db2","students","dyiheng,90,82,543,90.4");

       strcpy(cond[0].attrname,"name");
	   cond[0].op=EQ;
	   strcpy(cond[0].rhsValue,"neo");

	   strcpy(cond[1].attrname,"math");
	   cond[1].op=GE;
	   strcpy(cond[1].rhsValue,"50");

	   strcpy(cond[2].attrname,"name");
	   cond[2].op=NE;
	   strcpy(cond[2].rhsValue,"neo");
	   strcpy(yu.attrname,"eng");
	   Select("db2","students",0,NULL,NULL,1,-1,yu);
	   //Delete("db2","students",cond,1);
	   Update("db2","students",cond,1,"avg","100");
	   printf("\n\n\n\n\newe\n");
	   Select("db2","students",0,NULL,NULL,1,-1,yu);*/
	   intiStorage();
	   while(1)
	   {
		   char input[200]={0};
		   int i=0;
		   printf("simpledb>");
		   while( (input[i]=getchar())!='\n' && i<199)i++;
		   input[i]=' ';
		   TrimAllStr(input);
		   if(!strcmp(input,"exit"))exit(0);
		   //CommandParaser(input);
		   expleinWhereClause(input,NULL);
	   }
	  
   system("pause");
}


