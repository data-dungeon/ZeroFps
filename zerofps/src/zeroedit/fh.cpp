/*
	Offical FulHack.cpp fil =).
	For ZeroEdit.exe.

	All code in this file have to be inside name tags that state the name of he who dares to write
	code in this file.
*/

/*** Start: Your Name ***/
	// Ugly Code From Hell Here...
/*** End: Your Name ***/


#include "fh.h"
#include "fh.h"
#include "../zerofps/basic/zfbasicfs.h"

/*** Start: Vim ***/
/*
FHObject::FHObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_iObjectType=OBJECT_TYPE_DYNAMIC;
}

FHObject::FHObject(const char* szMadName)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	if(szMadName) {
}
		
	GetPos()		= Vector3(0,0,0);
	m_iObjectType	= OBJECT_TYPE_STATIC;
}


void FHObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue)
{

}*/




// FH - ZFVFS
/*char achTestString[128];

void TestFS_Write()
{
	strcpy(achTestString, "Yooo liksom =)");

	ZFVFile kFile;
	kFile.Open("vfstest.ost",0, true);
	kFile.Write(achTestString, 128,1);
	kFile.Close();

//	FILE* fp = fopen("../nisse.txt","wb");
//	fwrite("sdfwersdf", 5,1,fp);
//	fclose(fp);
	
}

void TestFS_Read()
{
	ZFVFile kFile;
	kFile.Open("vfstest.ost",0, false);
	kFile.Read(achTestString, 128,1);

	Console* pkcon = static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	pkcon->Printf("Text: %s",achTestString );
		
	kFile.Close();
}
*/
/*** End: Vim ***/
