#include "CmdSystem.h"


void CmdSystem::add(void* pAddress,char* aName,int iType) {
	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	
	variable *kNy=new variable;
	
//	kNy->aName=aName;
//	kNy->iType=iType;
//	kNy->pAddress=pAddress;
cout<<"hej"<<endl;
//	vector<int> kVars;
	kVars.push_back(34);
	cout<<"balle"<<endl;
}

void* CmdSystem::get(char* aName) {
	cout<<"getting "<<aName<<endl;


}

void CmdSystem::list(void) {
/*	for(int i=0;i<kVars.size();i++) {
		cout<<kVars[i]->aName<<" = ";
		
		switch(kVars[i]->iType) {
			case integer:
				cout<<*(int*)kVars[i]->pAddress;break;
		}
		cout<<endl;	
	}*/
}
