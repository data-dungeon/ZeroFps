#ifndef _BASIC_CMDSYSTEM_H_
#define _BASIC_CMDSYSTEM_H_

#include <vector>
#include <iostream>
#include <cstring>
#include "basic_x.h"
#include "os.h"
#include "zfobject.h"
#include "basicconsole.h"

using namespace std;

enum data_types {
	type_int,
	type_float,
	type_double,
	type_long	
};

struct variable {
	char* aName;				// In Game variable name 
	int iType;					// What type see enum data_types
	void* pAddress;				// Pointer to variable
};

struct funktion 
{
	char* aName;				// Name of function.
	void (*pAddress) (void);	// Address to function.
};

class BASIC_API CmdSystem : public ZFObject  {
	private:
		vector<variable*> kVars;		
		vector<funktion*> kFunks;		
				
		BasicConsole* m_pkCon;

	public:
		enum FuncId_e
			{
			FID_SET,
			FID_VARLIST
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		CmdSystem(void);
		
		void Add(void* pAddress,char* aName,int iType);		//add new ingame variable
		void AddCmd(void*,char* aName);
		void Get(char* aName);														//print variable aName
		void List(void);																	//list all variables
		bool Set(char* aName,double dData);								//set variable aName to dData
		bool Run(char* aName);
		inline vector<variable*> &GetList(void) {return kVars;};
		inline vector<funktion*> &GetCmdList(void) {return kFunks;};		
		double GetVar(int i);
};


#endif





