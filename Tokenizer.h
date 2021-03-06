/** 
* Tokenizer.h
* Amr Gamil
* GVM
*/

#pragma once

#include <tuple>
#include <map>
#include "Token_Type.h"
#include "Context.h"
#include "PE.h"
#include<boost/thread/mutex.hpp>

using namespace std;

class Core;
class Tokenizer;

struct RestoreArgs;
struct contextMapContent;
typedef vector<Token_Type*> Vector_token;
typedef unordered_map<unsigned long, RestoreArgs> RestorMapType;
typedef unordered_map<unsigned long long, contextMapContent> ContextMapType;

// structure to hold the context info for bind > 1
struct contextMapContent{
	Context *cx;
	short bds;
	short coreId;
};


// structure to hold the restored info
struct RestoreArgs{	
	// det chunk 
	short chunk;
	// dest instruction address
	int idx;
	// dest port number
	short port;
	// old context
	Context *cx;
	// number of expected return values
	short restores;
	// field to save the ex_coreID
	short ex_coreID;
};

/*
	class to handel the switch instruction special functionality
*/
class Switcher
{
public:
	Switcher();
	//Switcher(Tokenizer &tokenizer);
	~Switcher();

	// add an element to the map
	void addSwitchStorageElement(Token_Type *tok);

	Vector_token getAllElement(unsigned long long &tokenId);

	// store the selected dest recieved by tokens with port == 0,
	// in case there still exist tokens to come
	void storeDest(Token_Type *tok, short remainingInputs, int* indx);

	// update and do garbage collection if needed in the savedDestinations
	tuple<short, int*> updateStoredDest(Token_Type *tok);

	// check if port 0 token has already been recieved 
	bool alreadyExists(Token_Type *tok);

	// a storage map for switch instruction tokens	
	unordered_map<unsigned long long, Vector_token> switchStorage;

	// s storage that hold selected dest recieved by tokens with port == 0
	// to be saved for further tokens with same pair<long,long>
	map<pair<long,long>, tuple<short,int*>> savedDestinations;

	// Forward tokens to the tokenizer queue
	void sendToTokinzer(Vector_Tuple &dest, Vector_token &tokV);

	/*	fields	*/
	Tokenizer *tokenizer;
};

/*
 Class ContextManager That manages the context change and restore
*/
class ContextManager
{
public:
	//ContextManager(Tokenizer &tokenizer);
	ContextManager();
	~ContextManager();

	// 
	void bind_save(Token_Type &tok, int* destAdd, int* retAdd, short &binds, short rest, unsigned long &instIdx);

	short bind_send(Token_Type &tok, int* destAdd, short destPort
		, short sentPort, int* retAdd, short rest, Context* cx, short corId);	

	// restore the cx for the recieved tok
	void restore(Token_Type &tok);

	/*	fields	*/

	// A map to store the return address and the old context for
	// the new created context. The key will be the old context
	RestorMapType restoreMap; 

	// A map to store the new created context in case of multiple <binds>
	// i.e. when we have to send multiple arguments to the same function
	// The key here is the Old conxId
	ContextMapType contextMap;

	Tokenizer *tokenizer;	
};


/*
* This class defines the Tokenizer.
* The tokenizer is responsible for:
 - collecting the result out of an execution of an execution packet.
 - Wrap this result in a token.
 - Send it to the token queue for furthure processing.
*/
class Tokenizer
{
public:
	Tokenizer();
	~Tokenizer(void);	
	// create a new token and send it to the token queue
	void wrapAndSend(Vector_Tuple &distList, Datum &res, Context &cx, short &coreId, short token_executor_coreID);

	// The load distrubuter handler function
	short loadDistrubuter();

	// The load balanacer handler function
	void loadBalancer();

	// propogate the stop token
	void sendStop(Token_Type *tok);

	/*	fields	*/

	//vector of other cores in the system
	static vector<Core*> coreList;

	// the core that this tokenizer belongs to
	Core *core;

	Switcher swicther;
	ContextManager contextManager;

	boost::mutex tz_mx;
};
