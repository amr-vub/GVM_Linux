/** 
* GVM.cpp
* Amr Gamil
* GVM
*/
// GVM.cpp : Defines the entry point for the console application.
//

#include "Token_Type.h"
#include "Core.h"
#include "parser.h"
#include "IMemory.h"
#include "MatchingUnit.h"
#include "TokenDispatcher.h"
#include "Scheduler.h"
#include "Datum.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <boost/thread/thread.hpp>

short globalNum_ips;

short CORENUMBERS = 4;

using namespace std;

bool isLegalLine(string &line);
void printResult(Token_Type* tok);

int main(int argc, char* argv[])
{	
	clock_t start;
    double duration;		

	// open the instrucion file to parse
	fstream instFile;
	instFile.open(argv[1]);
	if(argc >= 3)
	  CORENUMBERS = atoi(argv[2]);
	
	string line;

	if(instFile.is_open())
	{
		// read the file line by line
		while(getline(instFile, line))
		{
			if(!isLegalLine(line))
				continue;
			parser::parserMain(line);
		}
	}
	// create a core per processor

	vector<Core*> coreList;
	for (int i = 0; i < CORENUMBERS; i++)
	{
		Core *core = new Core(i);
		core->tokenizer.coreList.push_back(core);
		coreList.push_back(core);
	}
	//printf("size : %d \n", coreList.front()->tokenizer.coreList.front()->coreID);
	Core *core = coreList.front()->tokenizer.coreList.front();
	// start instruction address
	int indxStrAdd[2] = {0,0};
	string input;
	
	// get a context
	Context *firstCx = core->conxObj.getUniqueCx(core->coreID);
	// get the user input, and for each, create a token
	for(int i=0;i<globalNum_ips;i++)
	{
		vector<string> strToks;
		getline(cin,input);
		parser::Tokenize(input,strToks);
		Tag * tag = new Tag(*firstCx, i, indxStrAdd, core->coreID);
		if(strToks[0] == "[]")
		{
		     	Datum datum = Datum();
			datum.token_Type = Datum::I_VECTOR;
			Token_Type *tok = new Token_Type(datum, tag);
			core->inbox.push_back(tok);  
		}
		else if(strToks.size() == 1)
		{			
			Datum datum = Datum(atoi(strToks[0].c_str()));
			datum.token_Type = Datum::INT;
			Token_Type *tok = new Token_Type(datum, tag);
			core->inbox.push_back(tok);
		}		
		else if(strToks.size() > 1)
		{
			Datum datum = Datum();
			datum.token_Type = Datum::I_VECTOR;
			for (int i = 0; i < strToks.size(); i++)
			{
				datum.iValue_v.push_back(atoi(strToks[i].c_str()));
			}						
			Token_Type *tok = new Token_Type(datum, tag);
			core->inbox.push_back(tok);
		}

	}

	start = std::clock();
	// start the actual work
	for (int i = 0; i < coreList.size(); i++)
	{
		coreList[i]->run();
	}
	// wait for all cores to finish
	for (int i = 0; i < coreList.size(); i++)
	{
		coreList[i]->stop();
		cout << "Core " << i << " is "<<coreList[i]->Idle_Counter << endl;
		cout << "Core " << i << " dependant_count is "<<coreList[i]->dependant_count << endl;
		cout << "Core " << i << " independant_count is "<<coreList[i]->independant_count << endl;
		cout << "Core " << i << " was active "<<coreList[i]->StartUp_Done << endl;
	}	
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	std::cout<<"Total time: "<< duration <<'\n';

	// when it returns from start, the token that resides in the inbox
	// will be the program result
	Token_Type *tok = core->inbox.front();	
	if(argc >= 4)
	  if(*argv[3] == 'T')
	      printResult(tok);
	//core->inbox.();

	// freeing memory
	delete tok;

	return 0;

}

// function to print the result
void printResult(Token_Type* tok)
{
        cout<< "the result is : \n";
	switch (tok->data.token_Type)
	{
	case Datum::I_VECTOR:
		{
			cout<< "[";
			for (int i = 0; i < tok->data.iValue_v.size(); i++)
			{
				cout<< tok->data.iValue_v[i] ;
				if(i + 1 != tok->data.iValue_v.size())
					cout<< ", " ;
			}
			cout<< "]"<<endl;
		}
		break;
	default:
		cout<< tok->data.iValue<<endl;
	}
}

/* Function to parse the line and skip if it starts with '$'
   ,which is the comment char in DIS, and to delete special chars e.g. \t (tabs)
*/
bool isLegalLine(string &line)
{
	for(string::iterator it = line.begin(); it<line.end();++it)
	{
		// this fixes the problem of "\t$ ........"
		if((*it) == '$')
			return false;
		// this fixed the problem of e.g. "\tINST...."
		while((*it) == '\t')
		{
			line.erase(it);
			if(it == line.end())
				break;
		}
		if(it == line.end())
			break;
	}
	return true;
}
