#ifndef _AABBTREE_H_
#define _AABBTREE_H_

#include "../../basic/plane.h"
#include "../propertys/p_mad.h"
#include <vector>

using namespace std;


class AABBTreeInfo
{
	public:
		int	m_iMaxDepth;
		int	m_iMaxTriangles;
};


class Triangle
{
	private:
		bool TestSides(const Vector3& kPos);
		bool TestLineVSSphere(const Vector3& kPos,float fRadius,const Vector3& kP1,const Vector3& kP2,Vector3* kTestPos);
		
	public:
		Vector3	m_kVerts[3];
		Vector3	m_kNormals[3];
		
		Plane		m_kPlane;		
		
		
		Vector3	Middle()		{	return (m_kVerts[0]+m_kVerts[1]+m_kVerts[2]) / 3.0;	};
		
		bool TestSphere(const Vector3& kPos,float fRadius,Vector3* kTestPos);
		bool TestLine(const Vector3& kP1,const Vector3& kP2,Vector3* kTestPos);

};

class AABB
{
	public:
		Vector3	m_kMin;
		Vector3	m_kMax;
		
		Plane		m_kSide[6];
		
		void Set(vector<Triangle>*	pkTriangles);
		int  LongestAxis();
		
		bool TestSphere(const Vector3& kPos,float fRadius);
		bool TestLine(const Vector3& kP1,const Vector3& kP2);
};

class AABBNode
{
	public:	
		AABB			m_kBox;
	
		AABBNode*	m_pkChild1;
		AABBNode*	m_pkChild2;		
				
		vector<Triangle>	m_kTriangles;
		
		AABBNode();
		~AABBNode();
		void Create(vector<Triangle>*	pkTriangles,int iDepth,const AABBTreeInfo& kInfo);				
		void GetAABBList(vector<AABB>* pkAABBList);
		
		bool TestSphere(const Vector3& kPos,float fRadius,Vector3* kTestPos);
		bool TestLine(const Vector3& kP1,const Vector3& kP2,Vector3* kTestPos);
};


class AABBTree
{
	private:
		AABBTreeInfo	m_kInfo;
		
	public:
		AABBNode*		m_pkRootNode;
	
		AABBTree();
		~AABBTree();

		bool IsValid()			{	return m_pkRootNode;	};		
		void Clear();
				
		void Create(vector<Mad_Face>* m_pkFaces, 
						vector<Vector3>* m_pkVertex, 
						vector<Vector3>* m_pkNormal, 
						const Matrix4& kRotation, 
						float fScale,
						const Vector3& kPos
						);
		
		void GetAABBList(vector<AABB>* pkAABBList);
};





#endif


