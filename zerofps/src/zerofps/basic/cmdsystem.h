#ifndef _BASIC_CMDSYSTEM_H_
#define _BASIC_CMDSYSTEM_H_

#include <vector>
#include <iostream>
#include <cstring>
#include <string.h>
#include "basic_x.h"
#include "os.h"
#include "zfobject.h"
#include "basicconsole.h"

using namespace std;

enum data_types {
	type_int,
	type_float,
	type_double,
	type_long,
	type_string
	
};

struct variable {
	char* aName;				// In Game variable name 
	int iType;					// What type see enum data_types
	void* pAddress;				// Pointer to variable
};



class BASIC_API CmdSystem : public ZFObject  {
	private:
		vector<variable*> kVars;		
						
		BasicConsole* m_pkCon;

	public:
		enum FuncId_e
			{
			FID_SET,
			FID_VARLIST
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		CmdSystem(void);
		
		void Add(void* pAddress,const char* aName,int iType);		//add new ingame variable
//		void Get(const char* aName);														//print variable aName
		void List();																	//list all variables
		bool Set(const char* aName,const char* acData);								//set variable aName to dData
		void SetString(int i,const char* acData);								//set variable aName to dData		
		void SetValue(int i,const char* acData);								//set variable aName to dData		
//		inline vector<variable*> &GetList(void) {return kVars;};
		void* GetVar(int i);
};


#endif





