#include "basic_x.h"
#include "os.h"
#include "cmdsystem.h"
#include "zfobjectmanger.h"
#include "globals.h"

CmdSystem::CmdSystem(void)
: ZFObject("CmdSystem") {
	g_ZFObjSys.Register_Cmd("set",FID_SET,this);
	g_ZFObjSys.Register_Cmd("varlist",FID_VARLIST,this);

}


void CmdSystem::Add(void* pAddress,char* aName,int iType) {
//	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	char* aVarName=new char[256];				//create space to store variable name
	strcpy(aVarName,aName);							//Copy aName to the alocated space
	Gemens(aVarName);										//convert variable name to only lowercases
	
	variable *kNy=new variable;					//create new variable entry
	

	kNy->aName=aVarName;								//set name pointer
	kNy->iType=iType;										//set variable type
	kNy->pAddress=pAddress;							//set varialbe data pointer

	kVars.push_back(kNy);								//push in to variable vector
}

void CmdSystem::AddCmd(void* pAddress,char* aName) {
//	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	char* aVarName=new char[256];				//create space to store variable name
	strcpy(aVarName,aName);							//Copy aName to the alocated space
	Gemens(aVarName);										//convert variable name to only lowercases
	
	funktion *kNy=new funktion;					//create new variable entry
	

	kNy->aName=aVarName;								//set name pointer
//	kNy->pAddress=pAddress;							//set varialbe data pointer

	kFunks.push_back(kNy);								//push in to variable vector
}


void CmdSystem::Get(char* aName) {
	Gemens(aName);											//convert searched name to lowercases
	for(int i=0;i<kVars.size();i++) {		//loop trough all variables in variable vector
		if(strcmp(kVars[i]->aName,aName)==0){	//if we find the variable
			cout<<aName<<" = "<<GetVar(i)<<endl;//print it
		}
	}
}

void CmdSystem::List(void) {
	for(int i=0;i<kVars.size();i++) {		//loop trough variable vector
		cout<<kVars[i]->aName<<" = "<<GetVar(i)<<endl;	//print all elements
	}
}

double CmdSystem::GetVar(int i) {
	double pdData;
	
	//check what type and convert the void pointer depending on that type
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
	
	//loop trough variable vector 
	for(int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){	//if we find the right variable
			found=true;
			switch(kVars[i]->iType) {		//set it depending on what type it is
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

bool CmdSystem::Run(char* aName) {
	Gemens(aName);
	bool found=false;
	
	//loop trough variable vector 
	for(int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){	//if we find the right variable
			found=true;
			cout<<"excuting "<<aName<<endl;		
		}
	}
	return found;
}	

void CmdSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
//	Console* pkCon = static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
//		Need to move console to basic.
	char name[256]="";
	char value[20]="";
	int i=4;		
	char text[255]="";

	switch(cmdid) {
		case FID_SET:

			if(kCommand->m_kSplitCommand[1].size()==0) {
				//Print("Please Supply a varible name");
				cout << "Please Supply a varible name" << endl;
				return;
			}

			if(kCommand->m_kSplitCommand[2].size()==0) {
				//Print("Please Supply a value");
				cout << "Please Supply a value" << endl;
				return;
			}
			
			
			strcpy(text,"Setting ");
			strcat(text,kCommand->m_kSplitCommand[1].c_str());
			strcat(text,"=");
			strcat(text,kCommand->m_kSplitCommand[2].c_str());
			cout << text << endl;
			
			strcat(name,kCommand->m_kSplitCommand[1].c_str());
			
			if(!Set(name,atof(kCommand->m_kSplitCommand[2].c_str()))){
				//Print("Variable not found");
				cout << "Variable not found" << endl;
				return;
			}
			
			break;

		case FID_VARLIST:
//			Print("");
			cout << endl;
			//Print("### variable list ###");
			cout << "### variable list ###" <<endl;
			for(int i=0;i<GetList().size();i++){
				strcpy(text,GetList()[i]->aName);
				strcat(text," = ");
				IntToChar(value,(int)GetVar(i));
				strcat(text,value);
	//			strcat(text,atoi(m_pkCmd->GetVar(i)))
	//			cout<<<<" = "<<m_pkCmd->GetVar(i)<<endl;
				//Print(text);	
				cout << text << endl;
			}

			break;
	}
}