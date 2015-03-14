/** 
* Core.cpp
* Amr Gamil
* GVM
*/

#include "Core.h"

boost::mutex Core::s_mutex;
bool Core::active = true;

// constructor
Core::Core(int corid)
{
	//this->active = false;
	this->coreID = corid;

	this->dispatcher.core = this;
	this->matchUnit.core = this;
	this->sch.core = this;
	this->tokenizer.core = this;

	this->total_inbox_size = 0;
	this->Idle_Counter = 0;
	this->independant_count = 0;
	this->dependant_count =0;
	// for now, just a RING topoogy with 2direct nieghboors, left and right
	this->nieghborList[0] = ( corid - 1 ) % CORENUMBERS;
	if(this->nieghborList[0] < 0)
		this->nieghborList[0] += CORENUMBERS;

	this->nieghborList[1] = ( corid + 1 ) % CORENUMBERS;
	if(this->nieghborList[1] >= CORENUMBERS)
		this->nieghborList[1] = 0;

}

Core::Core(void)
{	
}

//destructor
Core::~Core(void)
{
}

// Entry point for starting the core work
void Core::start()
{
	//this->active = true;
	int fake= 0;
	this->StartUp_Done = false;

	while(this->active)
	{			
		if(this->coreID == 0)
			fake++;
		if(this->inbox.size() != 0){
			/*
			if(this->Idle_Counter < this->inbox.size())
				this->Idle_Counter = this->inbox.size();*/
			// StartUp_Done is true now as the inbox got at least one element from master core
			this->StartUp_Done = true;
			Token_Type *tok = this->getScheduleElement();//this->inbox.front();
			//this->inbox.pop_back();			
			if(tok->tag->instAdd[0] == 1 && tok->tag->instAdd[1] == 13)
				cout<< "";
			this->dispatcher.dispatch(tok);
			//this->eraseToken();			
		}
		else if(this->get_ind_size() != 0)
		{
			/*
			if(this->Idle_Counter < this->ind_Inbox.size())
				this->Idle_Counter = this->ind_Inbox.size();
			*/	
			// StartUp_Done is true now as the inbox got at least one element from master core
			this->StartUp_Done = true;
			Token_Type *tok = this->getScheduleElement_IndInbox();//this->inbox.front();
			//this->inbox.pop_back();			
			this->dispatcher.dispatch(tok);
		}
		else
		{			
			// check if this idleness is not becasue of the startup
			if(this->StartUp_Done)
			{
				// now this core is idle after getting intial work from the master core
				// then load balancing has to be started
				this->tokenizer.loadBalancer();
				//fake++;
			}
			//boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
	}
	/*
	boost::lock_guard<boost::mutex> guard(s_mutex);
	cout << "core number: " << this->coreID << " was idle: " << fake << endl;
	*/
}

// spawn the thread
void Core::run()
{
	this->c_thread = boost::thread(&Core::start, this);
}

// wait for the thread to finish
void Core::stop()
{
	this->c_thread.join();
}

// insert tokens into the core's queue in a safe way
// TODO, till we use TBB concurrent vector
void Core::insertToken(Token_Type* tok)
{
	// first acquire the lock
	boost::lock_guard<boost::mutex> guard(c_mutex);
	inbox.push_back(tok);
	total_inbox_size++;
	this->dependant_count++;
}

// insert tokens into the core's independant queue in a safe way
void Core::insertToken_InIndInbox(Token_Type* tok)
{
	// first acquire the lock
	boost::lock_guard<boost::mutex> guard(ind_c_mutex);
	ind_Inbox.push_back(tok);
	total_inbox_size++;
	this->independant_count++;
}


// erase tokens into the core's queue
list<Token_Type*>::iterator Core::eraseToken(list<Token_Type*>::iterator index)
{
	boost::lock_guard<boost::mutex> guard(c_mutex);
	return inbox.erase(index);
}

// get the schedualed element of the queue and erase it afterwards
// TODO
Token_Type* Core::getScheduleElement()
{	
	boost::lock_guard<boost::mutex> guard(c_mutex);
	
	Token_Type* temp = this->inbox.front();
	this->inbox.pop_front();
	total_inbox_size--;

	return temp;
}

// get the schedualed element of the queue and erase it afterwards
// TODO
Token_Type* Core::getScheduleElement_IndInbox()
{	
	boost::lock_guard<boost::mutex> guard(ind_c_mutex);
	
	Token_Type* temp = this->ind_Inbox.front();
	this->ind_Inbox.pop_front();
	total_inbox_size--;

	return temp;
}

//getting the size of ind_Inbox
unsigned int Core::get_ind_size()
{
	boost::lock_guard<boost::mutex> guard(ind_c_mutex);
	unsigned int size = ind_Inbox.size();;
	return size;
  
}