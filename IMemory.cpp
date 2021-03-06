/** 
* IMemory.cpp
* Amr Gamil
* GVM
*/

#include "IMemory.h"
#include "Instruction.h"

ImemoryMapTyp IMemory::Imemory[2];

IMemory::IMemory()
{
	//core = cr;
}

// destructor
// Freeing the memory
// loop through the map, get the second element (Instruction*)
// and delete it. 
IMemory::~IMemory(void)
{
	for(ImemoryMapTyp::iterator it= Imemory[0].begin(); it!=Imemory[0].end();++it)
	{
		delete (*it).second;
	}
	Imemory[0].clear();
	for(ImemoryMapTyp::iterator it= Imemory[1].begin(); it!=Imemory[1].end();++it)
	{
		delete (*it).second;
	}
	Imemory[1].clear();
}

/* given an instruction idx, return the respective instruction
*	\param: 
		idx: instruction index
	\return
		the respective instruction
*/
Instruction* IMemory::get(int *indx)
{
	Instruction* inst = Imemory[indx[0]][indx[1]];
	
	return inst;
}

/*
* Add an entry to the program memory
	\param:
		chunk: memory chunk
	\param:
		idx: address of the inst in the memory
	\param:
		inst: the inserted instruction
*/
void IMemory::put(short chunk, int idx, Instruction* inst)
{
	Imemory[chunk][idx] = inst;
}