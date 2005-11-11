#ifndef _P_MD5_H
#define _P_MD5_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"
#include "../../zerofpsv2/basic/simplescript.h"
#include "../../zerofpsv2/basic/quaternion.h"

using namespace std;

class MD5AnimBone
{
public:
	Vector3		m_kPosition;
	Quaternion	kQuat;
};


class MD5Joint
{
public:
	string		m_strName;
	int			m_iParentIndex;
	Vector3		m_kPosition;
	Quaternion	kQuat;
};

class MD5Vertex
{
public:
	float		m_s, m_t;
	int		m_iViktIndex;
	int		m_iNumOfVikter;
};

class MD5Weight
{
public:
	int		m_iBone;
	float		m_fWeight;
	Vector3	m_kPosition;
};

class MD5Triangle
{
public:
	int		m_iIndex[3];
};

class MD5AnimJoint
{
public:
	string	m_strName;
	int		m_iParentIndex;
	int		m_iAnimFlags;
	int		m_iAnimCompIndex;
};

class MD5Bounds
{
public:
	Vector3	m_kMin;
	Vector3  m_kMax;
};

class MD5Frame
{
public:
	float*	m_fAnimComponents;
};

class MD5Anim
{
public:
	int		m_iNumOfFrames;
	int		m_iNumOfJoints;
	int		m_iFrameRate;
	int		m_iNumOfAnimComponents;

	MD5AnimJoint*		m_pkAnimJoints;
	MD5Bounds*			m_pkAnimBounds;
	MD5AnimBone*		m_pkAnimBaseFrame;
	MD5Frame*			m_pkFrame;

};


class P_Md5 : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		TextureManager*	m_pkTextureManger;
		ZSSRender*			m_pkRender;

		int					m_iNumOfJoints;
		int					m_iNumOfMesh;
		int					m_iNumOfVertex;
		int					m_iNumOfWeight;
		int					m_iNumOfTriangles;

		MD5Vertex*			m_pkVertex;
		MD5Joint*			m_pkJoint;
		MD5Weight*			m_pkWeight;
		MD5AnimBone*		m_pkBone;
		MD5Triangle*		m_pkTriangle;
		Vector3*				m_pkCalcVertex;

		int					m_iTexture;
		string				m_strFileName;

		MD5Anim*				m_kPlayingAnim;
		MD5Anim*				m_kAnim;
		vector<MD5Anim*>	m_pkAnim;

		int					m_iRenderFrame;
		float					m_fNextFrame;
		float					m_fLastFrame;

		int					m_iPlayAnim;

		bool HandleSetValue( const string& kValueName ,const string& kValue );

	public:
		P_Md5();
		~P_Md5();
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		

		void ReadMeshes(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void ReadJoints(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void HandleMeshFileCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void LoadMesh(string strFileName);
	
		void ReadAnim_Hierarchy(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void ReadAnim_Bounds(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void ReadAnim_BaseFrame(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void ReadAnim_Frame(SimpleScriptFile* pkScript, char* ucpTokenCmd);

		void HandleAnimFileCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd);
		void LoadAnimation(string strFileName);

		void DrawSkelleton_old();
		void DrawSkelleton();
		void DrawBone(Vector3 From, Vector3 To, Vector3 Color);
		void DrawSkin();

		void SetBindPose();
		void SetAnimFrame(int iFrame);
		void SetBaseFrame();

		void DrawBindPose();
};

Property* Create_P_Md5();
void Register_P_Md5(ZSSZeroFps* pkZeroFps);

#endif

