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
		double GetVar(int i);
		
		
	public:
		CmdSystem(void);
		
		void add(void* pAddress,char* aName,int iType);		
		void get(char* aName);
		void list(void);
		void set(char* aName,double);

};

#endif





