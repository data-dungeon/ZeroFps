#ifndef _ZSHADOW_H_
#define _ZSHADOW_H_

#include "../engine_systems_x.h"
#include "../../render/light.h"
#include "../../render/render.h"
#include "../../engine/zerofps.h"
#include "../propertys/p_mad.h"

using namespace std;

class	P_Mad;

class ENGINE_SYSTEMS_API ZShadow  : public ZFSubSystem
{
	private:
		Light*			m_pkLight;
		ZeroFps*			m_pkZeroFps;
		Render*			m_pkRender;
		EntityManager*	m_pkEntityMan;

		P_Mad*					m_pkMad;
		int*						m_pkFaces;
		Vector3*					m_pkVertex;
		int						m_iNrOfVerts;
		int						m_iNrOfFaces;

		//vector<Mad_Face>*		m_pkFaces;			// Faces in mesh.
		//vector<Vector3>*		m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>			m_kTransFormedVertexs;
		vector<pair<int,int> >	m_kTowardsEdges;
//		vector<Vector3>

		int			m_iCurrentShadows;
		int			m_iCurrentVerts;

		int			m_iStencilBits;
		bool			m_bHaveCheckedBits;		//have we checked that the stencil buffer is ok
		bool			m_bDisabled;				//is shadows disabled

		int			m_iNrOfShadows;			//number of shadows calculated per model
		float 		m_fExtrudeDistance;		//distance to extrude the siluet vertices
		bitset<8>	m_kShadowGroups;

		void FindFrontCaping(Vector3 kSourcePos);
		void FindSiluetEdges(Vector3 kSourcePos);
		void ExtrudeSiluet(Vector3 kSourcePos);
		void MakeStencilShadow(Vector3 kSourcePos);
		void DrawShadow(float fItensity);

		bool SetupMesh(P_Mad* pkMad);

		void SetupGL();
		void SetupStencilBuffer();

	public:

		ZShadow();
		void Update();

		void EnableShadowGroup(int i) {m_kShadowGroups[i] = true;};
		void DisableShadowGroup(int i) {m_kShadowGroups[i] = false;};

		int GetCurrentShadows() {return m_iCurrentShadows;};
		int GetCurrentVerts() 	{return m_iCurrentVerts;};


		bool StartUp();
		bool ShutDown();
		bool IsValid();


};


#endif








