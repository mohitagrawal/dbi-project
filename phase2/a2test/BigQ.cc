#include "BigQ.h"

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {


	// read data from in pipe sort them into runlen pages

    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe
	//DECLARE
	//Vector array

	//tempOrderMaker=sortOrder;
	//Initializing runs and counters
	//countPagesInRun=0
	totalRuns=0;
	countPagesInRun=0;
	vectorSize=0;
	numRecsDumped = 0;
	//setSortNDump to false and isPipeEmpty to false
	setsortNDump=false;
	isPipeEmpty=false;
	recordRead=new Record();
	tempRecord=new Record();
	myFile=new File();
	//bufferPage and allocate memory
	bufferPage=new Page();




	//**Initialize the thread in the BigQ constructor
	// Start reading from the pipe i.e. call pipe.remove put in recordRead

	//*********************Generating run
	//While true
	while(true)
	{
		in.Remove(recordRead);
		//if pipe is not empty read(recordRead!=NULL) from pipe.remove
		if(recordRead!=NULL)
		{
			//if countPagesInRun<runlength number of pages of records are not read
			if(countPagesInRun<runlen)
			{
				//countRecordsInRunlength++
				countRecordsInRunlength++;
				//Read into a bufferPage till the page is not full
				//if page is full then
				if(bufferPage->append(recordRead)==0)
				{
					//append its records into ***vector array for sorting and increment countPagesInRun and set and empty the page
					while(bufferPage->GetFirst(tempRecord))
						sortingArray.push_back(tempRecord);
					countPageInRun++;
					//since record append was fail earlier
					bufferPage->append(recordRead);
					//bufferPage->EmptyItOut();
				}
				//else meaning page is not full yet
			}
			//else meaning unsorted run is complete now sort the vector
			else
				//setSortNDump to true
				setSortNDump=true;
		//**************Sorting run
		}//else meaning pipe is empty now(readRecord ==NULL)
		else
		{
			//append records from bufferPage into vector array
			while(bufferPage->GetFirst(tempRecord))
				sortingArray.push_back(tempRecord);

			//setSortNDump to true
			setSortNDump=true;
						//setPipeEmpty to true
			setPipeEmpty=true;
		}
		//*********************************************************Sorting and dumping process
		//if setSortNDump is true&&bufferPage is not empty
		if(setSortNDump==true)
		{
			//totalRuns++
			totalRuns++;
			//function for sorting vector is sort() we can pass our compare function here
			sort(sortingArray.begin(),sortingArray.end(),compareRecords);
			//after sorting is done we need to dump this vector into file thru pages.
			//empty bufferPage

			bufferPage->EmptyItOut();
			//vectorSize = vector.size();
			vectorSize=sortingArray.size();


			//******************Dumping run into file
			//int numRecsDumped=0;
			//while(numRecsDumped<vectorSize)
			while(numRecsDumped<vectorSize)
			{
			//	read next record(i) from vector
				tempRecord=sortingArray.at(numRecsDumped);
				//if(append that record to page)
				if(bufferPage->Append(tempRecord))
					//numRecsDumped++;
					numRecsDumped++;


				//else append record fails that means page is full.
				else
				{
					//dump this temppage into file
					if(myFile->GetLength()==0)
						myFile->AddPage(bufferPage,0);
					else
						myFile->AddPage(bufferPage,myFile->GetLength()-1);
					//empty temp page
					bufferPage->EmptyItOut();
				}

			}
			sortingArray.empty();
			setSortNDump=false;
		//call vector.empty
		//setSortNDump to false

		}
				//************************************************************Sorting and dumping process ends
		//if isPipeEmpty is true
		if(isPipeEmpty==true)
			break;

			//break;

	//**To consider last page of the runLength(SOLVED)

	out.ShutDown ();
}

BigQ::~BigQ ()
{
	delete myFile;
}

bool BigQ :: CompareRecords(Record x, Record y)
{
	//Compare(Record *left, Record *right, OrderMaker *orderUs);
	ComparisonEngine compEng;
	if(compEng.Compare(x,y,sortorder)<0)
		return false;
	else
		return true;

}

