#pragma warning(disable : 4275)		
#include "p_mdlhl.h"


P_MdlHL::P_MdlHL() : Property("P_MdlHL")
{
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = true;	
	m_iVersion = 2;
	m_iSortPlace=4;
	
	m_pkTextureManger =	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));		
	LoadMdl("halflife/dawn.mdl");
}

P_MdlHL::~P_MdlHL()
{

}

void P_MdlHL::Update()
{

}


bool P_MdlHL::HandleSetValue( const string& kValueName ,const string& kValue )
{
/*	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) 
	{
		LoadModel(kValue);
		return true;
	}*/
	
	return false;
}


void P_MdlHL::Save(ZFIoInterface* pkPackage)
{
//	pkPackage->Write(m_fSize);
}

void P_MdlHL::Load(ZFIoInterface* pkPackage,int iVersion)
{
}


void P_MdlHL::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
//	pkNetPacket->Write(m_fSize);
	
//	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_MdlHL::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
//	pkNetPacket->Read(m_fSize);
}

void P_MdlHL::LoadMdl(string strFileName)
{
	FILE* fp = fopen(strFileName.c_str(),"rt");
	int iSize;

	fseek(fp, 0, SEEK_END);
	iSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	m_pcFileBuffer = new char [iSize];
	fread(m_pcFileBuffer, iSize, 1, fp);
	fclose(fp);

	m_pkHeader	= (HLHeader*) m_pcFileBuffer;
	m_pkBones	= (HLBone*) (m_pcFileBuffer + m_pkHeader->m_iBoneIndex);
	m_pkSeqDesc = (HLSequenceDesc*) (m_pcFileBuffer + m_pkHeader->m_iSequencesIndex);

	PrintHeader();
	PrintBones();
}

void P_MdlHL::PrintBones()
{
	cout << "m_pkBones->m_acName: " << m_pkBones->m_acName << endl; 
	cout << "m_pkBones->m_afScale: " << m_pkBones->m_afScale << endl; 
	cout << "m_pkBones->m_iParent: " << m_pkBones->m_iParent << endl; 
}

void P_MdlHL::PrintHeader()
{
	cout << "m_kHeader.m_cName: " << m_pkHeader->m_cName << endl; 
	cout << "m_kHeader.m_iNumOfBodyParts: " << m_pkHeader->m_iNumOfBodyParts << endl; 
	cout << "m_kHeader.m_iNumOfAttachments: " << m_pkHeader->m_iNumOfAttachments << endl; 
	cout << "m_kHeader.m_iNumOfBoneControllers: " << m_pkHeader->m_iNumOfBoneControllers << endl; 
	cout << "m_kHeader.m_iNumOfBones: " << m_pkHeader->m_iNumOfBones << endl; 
	cout << "m_kHeader.m_iNumOfHitBoxes: " << m_pkHeader->m_iNumOfHitBoxes << endl; 
	cout << "m_kHeader.m_iNumOfSegGroups: " << m_pkHeader->m_iNumOfSegGroups << endl; 
	cout << "m_kHeader.m_iNumOfTextures: " << m_pkHeader->m_iNumOfTextures << endl; 
}



Property* Create_P_MdlHL()
{
	return new P_MdlHL;
}

void Register_P_MdlHL(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_MdlHL", Create_P_MdlHL);					

	// Register Property Script Interface
}
