#include "cmdsystem.h"

CmdSystem::CmdSystem(void) {

}


void CmdSystem::Add(void* pAddress,char* aName,int iType) {
//	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	char* aVarName=new char[256];
	strcpy(aVarName,aName);
	Gemens(aVarName);	
	
	variable *kNy=new variable;
	

	kNy->aName=aVarName;
	kNy->iType=iType;
	kNy->pAddress=pAddress;

	kVars.push_back(kNy);
}

void CmdSystem::Get(char* aName) {
	Gemens(aName);
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

bool CmdSystem::Set(char* aName,double dData) {
	Gemens(aName);
	bool found=false;
	
	for(int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){
			found=true;
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
	return found;
}	
