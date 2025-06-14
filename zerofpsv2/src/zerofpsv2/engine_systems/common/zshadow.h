#ifndef _ZSHADOW_H_
#define _ZSHADOW_H_

#include "../engine_systems_x.h"
#include "../../render/light.h"
#include "../../render/render.h"
#include "../../engine/zerofps.h"
#include "../propertys/p_mad.h"

using namespace std;

class	P_Mad;

enum ENGINE_SYSTEMS_API eShadowMode
{
	ezFail = 0,
	ezPass = 1,
};

class ENGINE_SYSTEMS_API ShadowMesh
{
	public:
		bool						m_bUsed;
		int						m_iShadowMode;

		P_Mad*					m_pkMad;
		Vector3					m_kMadPos;
		float						m_fMadScale;
		Matrix3					m_kMadRotation;

		LightSource*			m_pkLightSource;
		Vector3					m_kLightPos;

		ZVertexBuffer*			m_pkExtrudedSiluetVB;
		ZVertexBuffer*			m_pkFrontCapingVB;
		ZVertexBuffer*			m_pkBackCapingVB;
		
		vector<Vector3>		m_kExtrudedSiluet;
		vector<Vector3>		m_kFrontCaping;
		vector<Vector3>		m_kBackCaping;

		ShadowMesh(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode);
		~ShadowMesh();
		bool Equals(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode);
};

class ENGINE_SYSTEMS_API ZShadow  : public ZFSubSystem
{
	private:
		enum FuncId_e
		{
			FID_ENABLESHADOWGROUP,
			FID_DISABLESHADOWGROUP,
			FID_LISTSHADOWGROUPS,
		};
	
		ZSSLight*		m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		ZSSRender*			m_pkRender;
		ZShaderSystem*	m_pkZShaderSystem;
		ZSSEntityManager*	m_pkEntityMan;
		
		
		ZMaterial*		m_pkShadowModel;
		ZMaterial*		m_pkShadowMap;
		ZMaterial*		m_pkDebug;

		bool						m_bHaveMesh;
		int*						m_pkFaces;
		Vector3*					m_pkVertex;
		int						m_iNrOfVerts;
		int						m_iNrOfFaces;

		vector<ShadowMesh*>	m_kShadowMeshs;

		vector<Vector3>			m_kTransFormedVertexs;
		vector<Vector3>			m_kExtrudedSiluet;
		vector<Vector3>			m_kFrontCaping;
		vector<Vector3>			m_kBackCaping;

		int			m_iCurrentShadows;
		int			m_iCurrentVerts;
		int			m_iCurrentActiveShadows;

		int			m_iStencilBits;
		bool			m_bHaveCheckedBits;		//have we checked that the stencil buffer is ok
		bool			m_bDisabled;				//is shadows disabled

		ConVar		m_kbVBO;
		ConVar		m_kiNrOfShadows;			//number of shadows calculated per model
		ConVar		m_kiShadowMode;
		ConVar		m_kiDebug;
		ConVar		m_kfShadowIntensity;

		float 		m_fExtrudeDistance;		//distance to extrude the siluet vertices
		float			m_fFrontCapOffset;
		//vector<bool>	m_kShadowGroups;
		ConVar		m_kbShadowGroups;
		
		void DrawCapings(ShadowMesh* pkShadowMesh);
		void DrawExtrudedSiluet(ShadowMesh* pkShadowMesh);
		void DrawShadow(float fItensity);
		void DrawGradedShadow(float fItensity,Vector3 kPos,Vector3 kDir);

		bool SetupMesh(P_Mad* pkMad);
		void GenerateShadowMesh(ShadowMesh* pkShadowMesh);

		ShadowMesh*	GetShadowMesh(P_Mad* pkMad,LightSource* pkLightSource);
		ShadowMesh* AddShadowMesh(P_Mad* pkMad,LightSource* pkLightSource);
		void SetUnusedMeshs();
		void ClearUnusedMeshs();

		void MakeStencilShadow(P_Mad* pkMad,LightSource* pkLightSource);		
		void SetupStencilBuffer();

		void RunCommand(int cmdid, const ConCommandLine* kCommand);

		//void SetupGL();
		//void FindCapings(Vector3 kSourcePos);
		//void ExtrudeSiluet(Vector3 kSourcePos);
		//void FindSiluetEdges(Vector3 kSourcePos);

	public:

		ZShadow();
		void Update();

		void EnableShadowGroup(int i) ;
		void DisableShadowGroup(int i);

		int GetNumShadowsPerModel() { return m_kiNrOfShadows.GetInt(); }

		bool IsShadowGroupEnabled(int iGroup) 
		{ 
			vector<bool> kBoolVec;
			kBoolVec = m_kbShadowGroups.GetBoolVector();
			if( iGroup >= 0 && iGroup < kBoolVec.size() ) 
				return kBoolVec[iGroup]; 
			else
				return false;
		}
		int GetCurrentActiveShadows() {	return m_iCurrentActiveShadows;		}
		int GetCurrentShadows() 		{	return m_iCurrentShadows;				}
		int GetCurrentVerts() 			{	return m_iCurrentVerts;					}
		int GetBuffertSize()				{	return int(m_kShadowMeshs.size());	}

		bool StartUp();
		bool ShutDown();
		bool IsValid();


};


#endif