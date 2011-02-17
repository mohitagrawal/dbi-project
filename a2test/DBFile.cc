#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <iostream>
#include <cstring>
using namespace std;

//***To Do getting number of records from Page class
// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {

	//myFile object initialize and allocates memory
	myFile = new File();
	currentPage = new Page();
	currentGetPage = new Page();

	//Initialize readRecordPointer as 0
	addCounter=0;
	readRecordPointer = 0;
	readPagePointer = 0;

	//set loadFlag=0;
	loadFlag = false;
	addFlag = false;
	isFileOpen=false;

	countGetNext = 0;

}


//**Make it virtual when you inherit into another class
DBFile::~DBFile(){
	delete currentGetPage;
	delete currentPage;
	delete myFile;
}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
//Call open method of file class with 0(for creating file) and file_path as argument
	myFile->Open(0,f_path);
	//**startup is dummy parameter(To be used in assignment 2)
//Return value 1 on success and 0 on failure
	isFileOpen = true;
	return 1;
	//Open of file method doesn't return anything(but checks if file is opened or not and closes the program when this happens)
	//set isFileOpen = true
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
//**File has to be created and opened before calling load function
	//**Loadpath is the table path from where you have to load the currently opened file object's file
	//**Check new memory to be allocated to record or not

	//***** Memory leak problem is there in load as per valgrind 6 blocks for region table and 80 byets for supplier table
	//add record into currentPage by instantiating record
	FILE *tableFile = fopen (loadpath,"r");
	if(tableFile==NULL){
		cout<<"Load::file is opened but nothing inside"<<endl;
	}


	Record *insertMe;
	insertMe = new Record();

	int loadCounter=0;
	//Check if DBfile is currently empty
	if(myFile->GetLength()==0){

		//If yes then add page at current length
		//Appending new data using SuckNextRecord function of file class
		while(insertMe->SuckNextRecord(&f_schema,tableFile)==1/*when record sucked successfully*/){

			//Check if page is already full
			if(currentPage->Append(insertMe)==0/*means page is full*/){

				//Dumping into file
				//If yes add it to the file and empty out the currentBufferPage
				myFile->AddPage(currentPage,myFile->GetLength()/*which is zero*/ );
				currentPage->EmptyItOut();

				//and add the record to currentBufferPage else keep on appending
				currentPage->Append(insertMe);
			}

			loadCounter++;
			//Otherwise record gets appended to the currentPage which gets done in the if condition
		}
		//delete insertMe;
		//insertMe = new Record();

	}	//if file is not empty then GetPage last page from file
	else{
		myFile->GetPage(currentPage,myFile->GetLength()-2/*which is last page of the file*/);

		//Appending new data using SuckNextRecord function of file class
		while(insertMe->SuckNextRecord(&f_schema,tableFile)==1/*when record sucked successfully*/){

			//Check if page is already full
			if(currentPage->Append(insertMe)==0/*means page is full*/){

				//Dumping into file
				//If yes add it to the file and empty out the currentBufferPage
				myFile->AddPage(currentPage,myFile->GetLength()-1 );
				currentPage->EmptyItOut();

				//and add the record to currentBufferPage else keep on appending
				currentPage->Append(insertMe);
			}

			loadCounter++;
			//delete insertMe;
			//insertMe = new Record();
		}

	}

	//If there is incomplete page and it is first page to be added
	if(myFile->GetLength()==0){

		myFile->AddPage(currentPage,myFile->GetLength());

	}else{

		//Add the incomplete current page into which records are read into the file at get length - 2
		myFile->AddPage(currentPage,myFile->GetLength()-1);

	}
		//set loadFlag =1

		loadFlag = true;

		cout<<endl<<"Load:: load counter"<<loadCounter<<endl;
		//deallocating memeoyrr
		delete insertMe;

}

int DBFile::Open (char *f_path) {
	//Open the file with file length parameter as 1(as file is already created in create)
	myFile->Open(1,f_path);

	//**if file is not there then error comes from file class code automatically.

	//**Can't return 0 because file.open function doesn't return anything
	//set isFileOpen=true
	isFileOpen = true;

	//Return value 1 on success
	return 1;
}

void DBFile::MoveFirst () {
	//**Use currentGetPage in Get Next

	//Check file length using GetLength
	//If length is 0 then pop error that cannot move to first record as file is empty and return
	if(myFile->GetLength()==0){
		std::cout<<endl<<"Move First :: File is empty. No record exists"<<endl;
	}
	else{
		//If file has something

		readPagePointer = 0;
		readRecordPointer =0;

		//empty the currentGetPage
		currentGetPage->EmptyItOut();

	}
	cout<<"Inside Move First \n";
	//else
		//**Set readRecordPointer as 0;

	//**fetch first page of file in currentGetPage(just pass 0 as second parameter when calling GetPage)

}

int DBFile::Close () {
//Close the DBfile using myFile object
	myFile->Close();
//myFile.close will return file length no. of pages
	readRecordPointer =0;
	//set readRecordPointer =0
	//set currentGetPage and currentPage =NULL
	currentGetPage = NULL;
	currentPage = NULL;
	//set addFlag and loadFlag =0;
	addFlag = false;
	loadFlag = false;
//return 1 on success
	if(!isFileOpen){
		cout<<endl<<"DBFile Close :: File cannot be closed as it is not opened"<<endl;
		return 0;
	}
	isFileOpen = false;
	return 1;
//Check if isFileOpen = true or false
	//
//cout<<"DBFile Close :: File cannot be closed as it is not opened";
	//return as failure(0) if false

}

void DBFile::Add (Record &rec) {
//**Check for isFileOpen flag
//** Valgrind on Supplier table => 80 bytes in 8 blocks
//** Valgrind on Region table => 40 bytes in 4 blocks
	//if file.getlength=0
	if(myFile->GetLength()==0){
	    //then no record exist then append record
		currentPage->Append(&rec);
	    //and dump page addpage(currentPage,getLength())
		myFile->AddPage(currentPage,myFile->GetLength());
	}
		//else check for addflag or loadflag=1 that means current page is available
		else{
			if((loadFlag==false)&&(addFlag==false)){
			myFile->GetPage(currentPage,myFile->GetLength()-2);
			}
				//append record to this available current page
				//if append returns 0 that means page is full and that page is already dumped
				if(currentPage->Append(&rec)==0){
			        //emptyItOut currentPage
					currentPage->EmptyItOut();
					//Append record to currentPage
					currentPage->Append(&rec);
					//dump currentPage to getlength -1
					myFile->AddPage(currentPage,myFile->GetLength()-1);

				}
				//else means append was successsful
				else{
					myFile->AddPage(currentPage,myFile->GetLength()-2);
					//dump currentPage to getlength -2
			}
		}
	addFlag=true;
	addCounter++;
	cout<<"Record Added: "<<addCounter<<endl;

	//DONE**what if we want to add record to empty file


	//**what if add and then load...what will happen to currentPage

}

//**Valgrind on Region.tbl => 40 bytes in 4 blocks
//**Valgrind on Supplier.tbl => 80 bytes in 8 blocks
int DBFile::GetNext (Record &fetchme) {
//Case1: file is empty

	//if readPagePointer ==0
	if(readPagePointer == 0){

		//if file.currentLength ==0
		if(myFile->GetLength()==0){

			//error"file is empty"
			std::cout<<endl<<"GetNext::File is empty"<<endl;
			return 0;

		}
		else{
		//else

			//Case2: file's first page
			//getPage from file with passing readPagePointer(0) as parameter into currentGetPag
			myFile->GetPage(currentGetPage,readPagePointer);

			//readPagePointer++
			//currentGetPage.getFirst(fetchme)
			//readRecordPointer++
			readPagePointer++;
			currentGetPage->GetFirst(&fetchme);
			readRecordPointer++;
			return 1;
		}
	}else{

		//Case3: file's middle page
		//if currentGetPage.Getfirst(fetchme) returns 0
		if(currentGetPage->GetFirst(&fetchme)==0){/*that is page becomes empty*/

			// currentGetPage.empty it out(Precautionary)
			//Precautionary
			currentGetPage->EmptyItOut();

			//Case4:last record of the file
			// if (currentLength-2)<= readPagePointer means we are on last page
			if((myFile->GetLength()-2)<=readPagePointer){

				//get Page from file call file.getPage(currentGetPage, readPagePointer-1)//get the same page from file
				myFile->GetPage(currentGetPage,readPagePointer-1);

				for(off_t i = 0;i< readRecordPointer;i++)//loop till we reach last read record
					currentGetPage->GetFirst(&fetchme);
				if(currentGetPage->GetFirst(&fetchme)==0){

					//(at last of the file)do this by returning appropriate values
					std::cout<<endl<<"Inside GetNext:: Reached end of file, nothing to read"<<endl;
                    return 0;
				}else{
					readRecordPointer++;
				    return 1;
				}
			}else{
				myFile->GetPage(currentGetPage,readPagePointer);
				readPagePointer++;
				readRecordPointer =0;
				currentGetPage->GetFirst(&fetchme);
				readRecordPointer++;
			    return 1;
			}
		}else{

			//Because get first is already executed in the checking condition of if
			readRecordPointer++;
			return 1;
		}

	}

}


int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
//Grow CNF's parse tree
	//
cout<<"Inside Get NExt\n";


	while(GetNext(fetchme)!=0) {

			if(comp.Compare (&fetchme, &literal, &cnf)) {
				cout<<"\n";
				countGetNext++;
				cout<<endl<<"GetNextCNF:: Compare value 1,count "<<countGetNext<<endl;
				return 1;
			}
		}
		return 0;

}
