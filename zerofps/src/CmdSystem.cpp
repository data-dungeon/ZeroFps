#include "CmdSystem.h"

CmdSystem::CmdSystem(void) {

}


void CmdSystem::Add(void* pAddress,char* aName,int iType) {
//	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	
	
	variable *kNy=new variable;
	
	kNy->aName=aName;
	kNy->iType=iType;
	kNy->pAddress=pAddress;

	kVars.push_back(kNy);
}

void CmdSystem::Get(char* aName) {
	for(int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){
			cout<<aName<<" = "<<GetVar(i)<<endl;			
		}
	}
}

void CmdSystem::List(void) {
	for(int i=0;i<kVars.size();i++) {
		cout<<kVars[i]->aName<<" = "<<GetVar(i)<<endl;
	}
}

double CmdSystem::GetVar(int i) {
	double pdData;
	
	switch(kVars[i]->iType) {
		case type_int:
			pdData= (double)*(int*)kVars[i]->pAddress;break;
		case type_float:
			pdData= (double)*(float*)kVars[i]->pAddress;break;
		case type_double:
			pdData= (double)*(double*)kVars[i]->pAddress;break;
		case type_long:
			pdData= (double)*(long*)kVars[i]->pAddress;break;
			
	}	
	return pdData;
}

void CmdSystem::Set(char* aName,double dData) {
	for(int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){
			switch(kVars[i]->iType) {
				case type_int:
					*(int*)kVars[i]->pAddress=(int)dData;break;
				case type_float:
					*(float*)kVars[i]->pAddress=(float)dData;break;
				case type_double:
					*(double*)kVars[i]->pAddress=(double)dData;break;
				case type_long:
					*(long*)kVars[i]->pAddress=(long)dData;break;
			
			}				
		}
	}
}	
