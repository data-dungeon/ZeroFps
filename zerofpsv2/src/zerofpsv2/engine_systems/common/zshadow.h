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

		vector<Vector3>		m_kExtrudedSiluet;
		vector<Vector3>		m_kFrontCaping;
		vector<Vector3>		m_kBackCaping;

		ShadowMesh(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode);
		bool Equals(P_Mad* pkMad,LightSource* pkLightSource,int iShadowMode);
};

class ENGINE_SYSTEMS_API ZShadow  : public ZFSubSystem
{
	private:
		Light*			m_pkLight;
		ZeroFps*			m_pkZeroFps;
		Render*			m_pkRender;
		EntityManager*	m_pkEntityMan;

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

		int			m_iDebug;
		int			m_iShadowMode;
		int			m_iNrOfShadows;			//number of shadows calculated per model
		float 		m_fExtrudeDistance;		//distance to extrude the siluet vertices
		float			m_fFrontCapOffset;
		bitset<8>	m_kShadowGroups;

		void DrawCapings(ShadowMesh* pkShadowMesh);
		void DrawExtrudedSiluet(ShadowMesh* pkShadowMesh);
		void DrawShadow(float fItensity);

		bool SetupMesh(P_Mad* pkMad);
		void GenerateShadowMesh(ShadowMesh* pkShadowMesh);

		ShadowMesh*	GetShadowMesh(P_Mad* pkMad,LightSource* pkLightSource);
		ShadowMesh* AddShadowMesh(P_Mad* pkMad,LightSource* pkLightSource);
		void SetUnusedMeshs();
		void ClearUnusedMeshs();

		void MakeStencilShadow(P_Mad* pkMad,LightSource* pkLightSource);

		void SetupGL();
		void SetupStencilBuffer();


		//void FindCapings(Vector3 kSourcePos);
		//void ExtrudeSiluet(Vector3 kSourcePos);
		//void FindSiluetEdges(Vector3 kSourcePos);

	public:

		ZShadow();
		void Update();

		void EnableShadowGroup(int i) {m_kShadowGroups[i] = true;};
		void DisableShadowGroup(int i) {m_kShadowGroups[i] = false;};

		int GetCurrentActiveShadows() {return m_iCurrentActiveShadows;};
		int GetCurrentShadows() {return m_iCurrentShadows;};
		int GetCurrentVerts() 	{return m_iCurrentVerts;};
		int GetBuffertSize()		{return m_kShadowMeshs.size();};

		bool StartUp();
		bool ShutDown();
		bool IsValid();


};


#endif








