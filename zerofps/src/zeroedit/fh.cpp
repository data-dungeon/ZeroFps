#include "fh.h"
#include "fh.h"
#include "../zerofps/basic/zfbasicfs.h"

FHObject::FHObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));


/*	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBasePtr(m_pkFps->GetMADPtr("../data/mad/tree.mad"));*/

	m_iObjectType=OBJECT_TYPE_DYNAMIC;
}

FHObject::FHObject(const char* szMadName)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	if(szMadName) {
//		AddProperty("MadProperty");
//		MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
//		madp->SetBasePtr(m_pkFps->GetMADPtr(szMadName));
		//szMadName
	}
		
	GetPos()		= Vector3(0,0,0);
	m_iObjectType	= OBJECT_TYPE_STATIC;
}


void FHObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue)
{

}




// FH - ZFVFS
char achTestString[128];

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
