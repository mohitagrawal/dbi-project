#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

class BigQ {
		int totalRuns,countPagesInRun;
		int vectorSize;
		int  numRecsDumped;
		Record* recordRead;
		Record* tempRecord;
		File* myFile;
		vector<Record*> sortingArray;
		Page* bufferPage;
		bool setSortNDump, isPipeEmpty;
public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
	bool CompareRecords(Record* left, Record* right);
};

#endif
