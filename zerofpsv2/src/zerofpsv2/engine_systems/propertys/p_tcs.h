#ifndef _P_TCS_H_
#define _P_TCS_H_

#include "../../engine/property.h"
#include <vector>
#include "../engine_systems_x.h"

#include "../tcs/tcs.h"

using namespace std;
class Tcs;

class ENGINE_SYSTEMS_API P_Tcs : public Property
{
	private:
		Tcs*	m_pkTcs;
			
		bool	m_bPolygonTest;
		float	 m_fRadius;						
		bool	m_bHavePolygonData;
		bool	m_bStatic;
		bool	m_bGravity;

		//mesh data
		int						m_iModelID;
		float 					m_fScale;
		vector<Mad_Face>*		m_pkFaces;			// Faces in mesh.
		vector<Vector3>*		 m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>*		 m_pkNormal;

		//temp data
		Vector3	m_kNewPos;
		Vector3	m_kMSPos;
		float		m_fMSRadius;	

		vector<PropertyValues> GetPropertyValues();
		float GetBoundingRadius();
		bool SetupMeshData();
	
	public:
		
		P_Tcs();
		~P_Tcs();		
		
		void Init();
		
		void CloneOf(Property* pkProperty) { }		
		void Update();
		
		void Enable();
		void Disable();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		
		void SetPolygonTest(bool t) {m_bPolygonTest = t;};
		void SetStatic(bool t) {m_bStatic = t;};
		void SetRadius(float t) {m_fRadius = t;};
		void SetRefetchPolygonData() {m_bHavePolygonData = false;};
		void SetGravity(bool t) {m_bGravity = t;};
		
		friend class Tcs;
};

Property* Create_P_Tcs();


#endif







