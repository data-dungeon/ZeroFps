#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <iostream>
#include <vector>
#include <cstring>

#include "../basic/basic.pkg"
#include "../engine/engine.pkg"

class ModellManager {
	private:
		struct modell {
			string file;
			HalfEdge *data;
		};
		
		FileIo *m_pkFile;
		vector<modell*> m_iModells;
		
		bool LoadModell(modell *mod,char *acFileName);
		bool LoadPmd(modell *mod,char *acFileName);
			
	public:
		ModellManager(FileIo* pkFile);
		bool Load(char* acFileName);

};




#endif




