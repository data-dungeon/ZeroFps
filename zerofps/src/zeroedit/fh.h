#ifndef _ZEROEDIT_FH_H_
#define _ZEROEDIT_FH_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "../zerofps/engine/engine.pkg"

class FHObject: public Object {
	private:
		ZeroFps *m_pkFps;

	public:
		FHObject();
		FHObject(const char* szMadName);
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
};

void TestFS_Write();
void TestFS_Read();

#endif





