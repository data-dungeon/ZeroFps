#ifndef _ENGINE_CSMECH_H_
#define _ENGINE_CSMECH_H_

#include "collisionshape.h"
#include "../engine_systems_x.h"
#include "cssphere.h"
#include "csbox.h"
#include "../propertys/p_mad.h"
#include "../mad/mad_core.h"

using namespace std;

class ENGINE_SYSTEMS_API CSMech : public CollisionShape
{
	private:
		Matrix4								 m_kModelMatrix;		
	
		Vector3								 m_kColPos;
		Vector3								 m_kGlideColPos;		
		Vector3 								 m_kColNormal;
		float 								 m_fcoloffset;

		bool 									 m_bHavePolygonData;
		
		bool 									 m_bOtherGlide;
		Vector3								 m_kOtherDest;
		
		Mad_Core* 							 m_pkCore;
		Mad_CoreMesh* 						 m_pkCoreMech;


		vector<Mad_Face>*					m_pkFaces;			// Faces in mesh.
		vector<Vector3>*					 m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>*					 m_pkNormal;

		list<int>							 m_kCollidedFaces;

	public:		
		int m_iModelID;	
		float m_fScale;
		
		CSMech();	
		Collision* Test(CollisionShape* kOther,bool bContinue);
		Collision* Collide_CSSphere(CSSphere* kOther);
		Collision* Collide_CSBox(CSBox* kOther);
		
		
		Vector3& Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		bool Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);		
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2,float fR);
		
		bool TestLineVSPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);		
		bool TestAxisBox(Vector3 kPos1,Vector3 kPos2,Vector3 kScale);
		
		bool BoxEdgeTest(Vector3 kPos,Vector3 kScale);
		
		bool SetUpMech();		
		void GenerateModelMatrix();
		
};


#endif

