#ifndef _ENGINE_CSMECH_H_
#define _ENGINE_CSMECH_H_

#include "collisionshape.h"
#include "../basic/basic.pkg"
#include "engine_x.h"
#include "cssphere.h"
#include "madproperty.h"
#include "mad_core.h"

using namespace std;

class ENGINE_API CSMech : public CollisionShape
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
		Collision* Test(CollisionShape* kOther,float fTime,bool bContinue);

		Collision* Collide_CSSphere(CSSphere* kOther,float fTime);
		
		Vector3& Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		bool Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2,float fR);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
		
		bool SetUpMech();
		
		void GenerateModelMatrix();
		
};


#endif

