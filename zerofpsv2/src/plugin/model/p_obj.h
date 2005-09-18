#ifndef _P_OBJ_H
#define _P_OBJ_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"
#include "../../zerofpsv2/basic/simplescript.h"

using namespace std;

class ObjFace
{
public:
	int	m_iMaterialIndex;
	int	m_iVertexIndex[3];
	int	m_iTextureIndex[3];
	int	m_iNormalIndex[3];
};

class MayaMtl
{
public:
	string	m_strName;
	string	m_strDiffuseColorTexture;
	Vector3	m_kAmbientColor;
	int		m_iTextureIndex;
};

class P_Obj : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;
		TextureManager*		m_pkTextureManger;

		vector<PropertyValues> GetPropertyValues();

		string				m_strFileName;

		vector<Vector3>		m_kVertex;
		vector<Vector3>		m_kTexture;
		vector<Vector3>		m_kNormal;
		vector<ObjFace>		m_kFace;

		bool				m_bNonTriangles;

	public:
		int					m_iCurrentMaterial;

		vector<MayaMtl>		m_kMayaMat;
		MayaMtl				m_kMaterial;
		void P_Obj::SaveMaterials();
		MayaMtl* P_Obj::GetMaterial(string strName);
		int	GetMaterialIndex(string strName);
		string m_szTextureName;

		P_Obj();
		~P_Obj();
		
		void Update();

		void Save(ZFIoInterface* pkPackage)									{}
		void Load(ZFIoInterface* pkPackage,int iVersion)				{}
		void PackTo(NetPacket* pkNetPacket, int iConnectionID )		{}
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID )	{}	
		void LoadModel(string strFileName);
		bool HandleSetValue( const string& kValueName ,const string& kValue );
		void HandleObjCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd);

		void P_Obj::HandleMtlCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void P_Obj::ReadMtlFile(char* szFileName);
};

Property* Create_P_Obj();
void Register_P_Obj(ZeroFps* pkZeroFps);

#endif

