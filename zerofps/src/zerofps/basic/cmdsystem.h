#ifndef _CMDSYSTEM_H_
#define _CMDSYSTEM_H_

#include "basic_x.h"
#include <vector>
#include <iostream>
#include <cstring>
#include "globals.h"

using namespace std;

enum data_types {
	type_int,
	type_float,
	type_double,
	type_long	
};

struct variable {
	char* aName;											//ingame variable name 
	int iType;												//what type see enum data_types
	void* pAddress;										//pointer to variable
};

struct funktion {
	char* aName;
	void (*pAddress) (void);
//	void* pAddress;
};

class BASIC_API CmdSystem {
	private:
		vector<variable*> kVars;		
		vector<funktion*> kFunks;		
				
	public:
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





