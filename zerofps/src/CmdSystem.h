#ifndef _CMDSYSTEM_H_
#define _CMDSYSTEM_H_

#include <vector>
#include <iostream>
#include <cstring>

using namespace std;

enum data_types {
	type_int,
	type_float,
	type_double,
	type_long	
};

struct variable {
	char* aName;
	int iType;
	void* pAddress;
};


class CmdSystem {
	private:
		vector<variable*> kVars;		
		
	public:
		CmdSystem(void);
		
		void Add(void* pAddress,char* aName,int iType);		
		void Get(char* aName);
		void List(void);
		void Set(char* aName,double);
		inline vector<variable*> &GetList(void) {return kVars;};
		double GetVar(int i);
};

#endif





