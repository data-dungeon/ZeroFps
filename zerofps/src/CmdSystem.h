#ifndef _CMDSYSTEM_H_
#define _CMDSYSTEM_H_

#include <vector>
#include <iostream>

using namespace std;

enum data_types {
	integer,
	floatingpoint
};



struct variable {
	char* aName;
	int iType;
	void* pAddress;
};


class CmdSystem {
	private:
		vector<int> kVars;
		
	public:
		CmdSystem(void);
		
		void add(void* pAddress,char* aName,int iType);
		
		void* get(char* aName);
		void list(void);

};

#endif





