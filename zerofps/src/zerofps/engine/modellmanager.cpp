#include "modellmanager.h"

ModellManager::ModellManager(FileIo* pkFile){
	m_pkFile=pkFile;
	
}

bool ModellManager::LoadModell(modell *mod,char* acFileName){
	
	char sufix[4];
	strncpy(sufix,&acFileName[strlen(acFileName)-4],4);
	Gemens(sufix);
	if(strcmp(sufix,".pmd")==0) 
		if(!LoadPmd(mod,acFileName))
			return false;


	return true;
}

bool ModellManager::LoadPmd(modell *mod,char* acFileName) {
//	mod->data=new HalfEdge;
	


}


bool ModellManager::Load(char* acFileName) {
	//first check if the modell is loaded
	for(int i=0;i<m_iModells.size();i++){
		if(m_iModells[i]->file==acFileName){
			//return true if it is
			return true;
		}		
	}

	//else create a new modell 
	modell *temp=new modell;
	temp->file=acFileName;
	
	//load the modell into it 
	if(!LoadModell(temp,acFileName)) {
		cout<<"Error Loading modell: "<<acFileName<<endl;
		return false;
	}
	
	//add it to the modell vector
	m_iModells.push_back(temp);
	cout<<"Loaded modell: "<<m_iModells.back()->file<<endl;
	
	//return true when the modell is loaded
	return true;
}


