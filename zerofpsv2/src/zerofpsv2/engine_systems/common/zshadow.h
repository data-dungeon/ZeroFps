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
		vector<Vector3>		m_kTransFormedVertexs;
		vector<pair<int,int> >	m_kTowardsEdges;


		int	m_iNrOfShadows;		//number of shadows calculated per model
		float m_fExtrudeDistance;

		void FindSiluetEdges(Vector3 kSourcePos);
		void ExtrudeSiluet(Vector3 kSourcePos);
		void MakeStencilShadow(Vector3 kSourcePos);
		void DrawShadow();

		bool SetupMesh(P_Mad* pkMad);

	public:

		ZShadow();
		void Update();

		bool StartUp();
		bool ShutDown();
		bool IsValid();


};


#endif








