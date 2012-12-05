struct tableBuffer
{
	char tbname[20];
	char dbname[20];
	struct table *pointer;
};

struct dbBuffer
{
	char dbname[20];
	struct database *pointer;
};