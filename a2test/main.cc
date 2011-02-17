#include <iostream>
#include "Record.h"
#include "DBFile.h"
#include "Pipe.h"
#include "BigQ.h"
#include <cstdlib>
using namespace std;
extern "C" {
	int yyparse(void);
}
extern struct AndList *final;

int main()
{
	//DBFile myDBFile;
	Record insertMe;
	Pipe inputpipe(2000);
	Pipe outputpipe(2000);
	int suckCounter=0;
	//Schema mySchema("catalog","customer");
	Schema *mySchema2=new Schema("catalog","customer");
	OrderMaker sortorder(mySchema2);
	FILE *myFile = fopen("/home/mohit/Desktop/10M/customer.tbl","r");
	while(insertMe.SuckNextRecord(mySchema2,myFile)==1)
	{
		suckCounter++;
		cout<<"Suck Counter "<<suckCounter<<endl;
		inputpipe.Insert(&insertMe);

	}

	BigQ temp(inputpipe,outputpipe,sortorder,3);
}
