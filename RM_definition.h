#include<stdio.h>

enum DataType
{
	INT,
	FLOAT,
	VARCHAR
};

enum Operator
{
	EQ,
	LT,
	GT,
	LE,
	GE,
	NE
};


struct colome;
typedef struct colome col;
struct colome
{
	char name[20];
    enum DataType type;
	int strLength;
};


struct table;
typedef struct table tb;
struct table
{
   char name[20];
   void *firstRecord;
   struct  colome *firstColome;
   int isDelete;
   int colomeCounts;
   int recordCounts;
   int recordCapacity;
};

typedef struct database
{
	char dbname[20];
    struct table *firstTable;
	int isDelete;
	int tableCounts;
	int tableCapacity;
	struct colome *colomes;
	int colomeCounts;
	int colomeCapacity;
};

typedef struct value
{
	char values[100];
};

typedef struct relAttr
{
	char attrname[20];
};

typedef struct Condition
{
	char attrname[20];
	enum Operator op;
	int bRhsIsAttr;
	char rhsAttr[20];
	char rhsValue[100];
};

typedef struct dataColome
{
	enum DataType type;
	void *value;
	int  strLength;
};
