#include "basic_x.h"
#include "os.h"
#include "cmdsystem.h"
#include "zfobjectmanger.h"
#include "globals.h"

CmdSystem::CmdSystem()
: ZFObject("CmdSystem") {
	g_ZFObjSys.Register_Cmd("set",FID_SET,this, "set name value", 2);
	g_ZFObjSys.Register_Cmd("varlist",FID_VARLIST,this);

	m_pkCon = dynamic_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));

}

/*
void CmdSystem::Add(void* pAddress,const char* aName,int iType) {
	ZFCmdDataType eType = CSYS_NONE;
	switch(iType) {
		case type_int:		eType = CSYS_INT;		break;
		case type_float:	eType = CSYS_FLOAT;		break;
		case type_double:	eType = CSYS_DOUBLE;	break;
		case type_long:		eType = CSYS_LONG;		break;
		case type_string:	eType = CSYS_STRING;	break;
		};


	g_ZFObjSys.RegisterVariable(const_cast<char*>(aName), pAddress,eType);
}
*/
	/*	
	//	cout<<"adding "<<aName<<" "<<iType<<" : "<<pAddress<<endl;
	char* aVarName=new char[256];				//create space to store variable name
	strcpy(aVarName,aName);							//Copy aName to the alocated space
	Gemens(aVarName);										//convert variable name to only lowercases
	
	variable *kNy=new variable;					//create new variable entry
	

	kNy->aName=aVarName;								//set name pointer
	kNy->iType=iType;										//set variable type
	kNy->pAddress=pAddress;							//set varialbe data pointer

	kVars.push_back(kNy);								//push in to variable vector
	*/
//}

/*
void CmdSystem::Get(const char* aName) {
	Gemens(aName);											//convert searched name to lowercases
	for(int i=0;i<kVars.size();i++) {		//loop trough all variables in variable vector
		if(strcmp(kVars[i]->aName,aName)==0){	//if we find the variable
			if(kVars[i]->iType == type_string)
				m_pkCon->Printf("%s = %s", aName, ((string*)GetVar(i))->c_str());
			else 
				m_pkCon->Printf("%s = %d", aName, *(double*)GetVar(i));
			//cout<<aName<<" = "<<GetVar(i)<<endl;//print it
		}
	}
}
*/

/*
void CmdSystem::List() {
	for(unsigned int i=0;i<kVars.size();i++) {		//loop trough variable vector
		cout<<kVars[i]->aName<<" = "<<GetVar(i)<<endl;	//print all elements
	}
}*/

/*
void* CmdSystem::GetVar(int i) {
//	double pdData;
	string pkData;
	
	
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
		case type_string:
			return (void*)kVars[i]->pAddress;
	}	
	
	
	return (void*)kVars[i]->pAddress;
//	return (void*)&pdData;
}
*/
bool CmdSystem::Set(const char* acName,const char* acData)
{
	return g_ZFObjSys.SetVariable(const_cast<char*>(acName),const_cast<char*>(acData));

/*	char aName[50];
	strcpy(aName,acName);
	Gemens(aName);
	
	//loop trough variable vector 
	for(unsigned int i=0;i<kVars.size();i++) {
		if(strcmp(kVars[i]->aName,aName)==0){	//if we find the right variable
			if(kVars[i]->iType == type_string) {
				SetString(i,acData);											
			}else {
				SetValue(i,acData);
			}
			
			return true;
		}
	}*/
	
	return false;
}


/*

void CmdSystem::SetValue(int i,const char* acData) {
	
	float dData = float(atof(acData));
	
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

void CmdSystem::SetString(int i,const char* acData) {
	(*(string*)kVars[i]->pAddress)=acData;
}
*/
void CmdSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_SET:
			if(!Set(kCommand->m_kSplitCommand[1].c_str(),&kCommand->m_strFullCommand.c_str()[kCommand->m_kSplitCommand[0].length() + kCommand->m_kSplitCommand[1].length() + 2])){
				m_pkCon->Printf("Variable not found");
				return;
			} else {
				m_pkCon->Printf("Setting %s = %s",kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str());
			}
			
			
			break;

		case FID_VARLIST:
			m_pkCon->Printf("### variable list ###");
			g_ZFObjSys.PrintVariables();
			break;
	}
}










/*			for(unsigned int i=0;i<kVars.size();i++)
			{
				switch(kVars[i]->iType)
				{
					case type_string:
						m_pkCon->Printf(" %s = [%s]",kVars[i]->aName,((string*)GetVar(i))->c_str());break;
					case type_float:
						m_pkCon->Printf(" %s = [%.3f]",kVars[i]->aName,*(float*)GetVar(i));break;
					case type_int:
						m_pkCon->Printf(" %s = [%i]",kVars[i]->aName,*(int*)GetVar(i));break;
					case type_double:
						m_pkCon->Printf(" %s = [%.3d]",kVars[i]->aName,*(double*)GetVar(i));break;
					case type_long:
						m_pkCon->Printf(" %s = [%l]",kVars[i]->aName,*(long*)GetVar(i));break;										
				}
			}*/

