#include "aabbtree.h"


//---- AABBTree
AABBTree::AABBTree()
{
	m_pkRootNode = NULL;

	m_kInfo.m_iMaxDepth 		= 50;
	m_kInfo.m_iMaxTriangles = 5;

}

AABBTree::~AABBTree()
{
	Clear();
}

void AABBTree::Clear()
{
	if(m_pkRootNode)
		delete m_pkRootNode;
}

void AABBTree::Create(	vector<Mad_Face>* m_pkFaces, 
								vector<Vector3>* m_pkVertex, 
								vector<Vector3>* m_pkNormal, 
								const Matrix4& kRotation, 
								float fScale
							)
{

	//create triangle list
	vector<Triangle>	kTriangles;	
	Triangle kTemp;
	
	int iSize = m_pkFaces->size();
	for(int f= 0;f<iSize;f++)
	{
		//transform vertex
		kTemp.m_kVerts[0] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[0]] );
		kTemp.m_kVerts[1] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[1]] );
		kTemp.m_kVerts[2] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[2]] );	
		kTemp.m_kVerts[0] *= fScale;
		kTemp.m_kVerts[1] *= fScale;
		kTemp.m_kVerts[2] *= fScale;	
		
		//setup triangles plane
		kTemp.m_kPlane.Set(kTemp.m_kVerts[0],kTemp.m_kVerts[1],kTemp.m_kVerts[2]);
		
		kTriangles.push_back(kTemp);
	}

	//clear list tree
	Clear();
		
	//create new root node
	m_pkRootNode = new AABBNode;
	m_pkRootNode->Create(&kTriangles,0,m_kInfo);
}

void AABBTree::GetAABBList(vector<AABB>* pkAABBList)
{
	if(m_pkRootNode)
		m_pkRootNode->GetAABBList(pkAABBList);

}

//---- AABBNode

AABBNode::AABBNode()
{
	m_pkChild1 = NULL;
	m_pkChild2 = NULL;
}

AABBNode::~AABBNode()
{
	if(m_pkChild1)
		delete m_pkChild1;
	if(m_pkChild2)
		delete m_pkChild2;
}


void AABBNode::Create(vector<Triangle>*	pkTriangles,int iDepth,const AABBTreeInfo& kInfo)
{
	//setup AABB
	m_kBox.Set(pkTriangles);	

	//do we want to create childs
	if(pkTriangles->size() > kInfo.m_iMaxTriangles && iDepth < kInfo.m_iMaxDepth)
	{
		//in wich axis shuld we split the mesh?
		int iLongestAxis = m_kBox.LongestAxis();	
		
		//calculate geometri avrage
		Vector3 kGeoAvrageCenter(0,0,0);				
		int iSize = pkTriangles->size();
		for(int i = 0;i<iSize;i++)
			kGeoAvrageCenter += (*pkTriangles)[i].Middle();
		kGeoAvrageCenter *= 1.0 / iSize;
			
		//split polygons
		vector<Triangle>	kChildBucket1;
		vector<Triangle>	kChildBucket2;			
		for(int i = 0;i<iSize;i++)
		{
			if( (*pkTriangles)[i].Middle()[iLongestAxis] > kGeoAvrageCenter[iLongestAxis])
				kChildBucket2.push_back((*pkTriangles)[i]);
			else
				kChildBucket1.push_back((*pkTriangles)[i]);				
		}
		
		
		//create childs
		m_pkChild1 = new AABBNode();
		m_pkChild2 = new AABBNode();
		
		m_pkChild1->Create(&kChildBucket1,iDepth+1,kInfo);
		m_pkChild2->Create(&kChildBucket2,iDepth+1,kInfo);
	}
	else
	{		
		// copy triangles to this node
		m_kTriangles = *pkTriangles;
		
		m_pkChild1=NULL;
		m_pkChild2=NULL;
	}
}

void AABBNode::GetAABBList(vector<AABB>* pkAABBList)
{
	//only add leaves

	if(m_pkChild1)
	{
		m_pkChild1->GetAABBList(pkAABBList);
		m_pkChild2->GetAABBList(pkAABBList);		
	}
	else	
		pkAABBList->push_back(m_kBox);

}

bool AABBNode::TestLine(const Vector3& kP1,const Vector3& kP2,Vector3* pkTestPos)
{
	//line collides with AABB?
	if(m_kBox.TestLine(kP1,kP2))
	{
		//we have childs
		if(m_pkChild1)
		{
			Vector3 kPos1;
			Vector3 kPos2;
			bool bHaveHit1 = false;
			bool bHaveHit2 = false;
					
			//test against each child
			if(m_pkChild1->TestLine(kP1,kP2,&kPos1))
				bHaveHit1 = true;

			if(m_pkChild2->TestLine(kP1,kP2,&kPos2))
				bHaveHit2 = true;
				
			//both tests was true, check wich was closest
			if(bHaveHit1 && bHaveHit2)
			{
				if(kPos1.DistanceTo(kP1) < kPos2.DistanceTo(kP1))
					*pkTestPos = kPos1;
				else
					*pkTestPos = kPos2;			
			
				return true;
			}
			
			//did hit child nr 1?
			if(bHaveHit1)
			{
				*pkTestPos = kPos1;
				return true;
			}	
			
			//did hit child nr 2?
			if(bHaveHit2)
			{
				*pkTestPos = kPos2;
				return true;
			}						
		}
		else
		{
			// no childs , assuming leaf, doing line VS triangle tests
		
			static float 	fClosest;
			static float 	fDistance;
			static bool  	bHaveColided;		
			static Vector3	kTempPos;
		
			bHaveColided = false;
			fClosest = 99999999;
			
			int iSize = m_kTriangles.size();
			for(int i = 0;i<iSize;i++)
			{
				if(m_kTriangles[i].TestLine(kP1,kP2,&kTempPos))
				{
					fDistance = kP1.DistanceTo(kTempPos);
					if(fDistance < fClosest)
					{
						bHaveColided = true;
						fClosest = fDistance;	
						*pkTestPos = kTempPos;
					}
				}
			}
			
			
			return bHaveColided;
		}	
	}
	
	
	return false;
}

bool AABBNode::TestSphere(const Vector3& kPos,float fRadius,Vector3* pkTestPos)
{
	//sphere collides with AABB?
	if(m_kBox.TestSphere(kPos,fRadius))
	{
		//we have childs
		if(m_pkChild1)
		{
			Vector3 kPos1;
			Vector3 kPos2;
			bool bHaveHit1 = false;
			bool bHaveHit2 = false;
					
			//test against each child
			if(m_pkChild1->TestSphere(kPos,fRadius,&kPos1))
				bHaveHit1 = true;

			if(m_pkChild2->TestSphere(kPos,fRadius,&kPos2))
				bHaveHit2 = true;
				
			//both tests was true, check wich one is closest
			if(bHaveHit1 && bHaveHit2)
			{
				if(kPos1.DistanceTo(kPos) < kPos2.DistanceTo(kPos))
					*pkTestPos = kPos1;
				else
					*pkTestPos = kPos2;			
			
				return true;
			}
			
			//did hit child nr 1?
			if(bHaveHit1)
			{
				*pkTestPos = kPos1;
				return true;
			}
			
			//did hit child nr 2?
			if(bHaveHit2)
			{
				*pkTestPos = kPos2;
				return true;
			}			
			
		}
		else
		{		
			static float 	fClosest;
			static float	fDistance;
			static Vector3	kTempPos;
			static bool		bHaveCollided;
			
			bHaveCollided 	= false;
			fClosest 		= 999999999;
			
			int iSize = m_kTriangles.size();
			for(int i = 0;i<iSize;i++)
			{
				if(m_kTriangles[i].TestSphere(kPos,fRadius,&kTempPos))
				{
					fDistance = kPos.DistanceTo(kTempPos);
		
					if( fDistance < fClosest)
					{
						bHaveCollided = true;
						fClosest = fDistance;
 						*pkTestPos = kTempPos;
					}
				}			
			}

			
 			return bHaveCollided;
		}
	}
		
		
	return false;
}





//---- AABB

void AABB::Set(vector<Triangle>*	pkTriangles)
{
	m_kMin.Set(999999999,99999999,99999999);
	m_kMax.Set(-999999999,-99999999,-99999999);

	//find max/min values
	int iSize = pkTriangles->size();
	for(int i =0;i<iSize;i++)
	{
		for(int j = 0;j<3;j++)
		{
			//min
			if( (*pkTriangles)[i].m_kVerts[j].x < m_kMin.x)
				m_kMin.x = (*pkTriangles)[i].m_kVerts[j].x;
			if( (*pkTriangles)[i].m_kVerts[j].y < m_kMin.y)
				m_kMin.y = (*pkTriangles)[i].m_kVerts[j].y;
			if( (*pkTriangles)[i].m_kVerts[j].z < m_kMin.z)
				m_kMin.z = (*pkTriangles)[i].m_kVerts[j].z;

			//max
			if( (*pkTriangles)[i].m_kVerts[j].x > m_kMax.x)
				m_kMax.x = (*pkTriangles)[i].m_kVerts[j].x;
			if( (*pkTriangles)[i].m_kVerts[j].y > m_kMax.y)
				m_kMax.y = (*pkTriangles)[i].m_kVerts[j].y;
			if( (*pkTriangles)[i].m_kVerts[j].z > m_kMax.z)
				m_kMax.z = (*pkTriangles)[i].m_kVerts[j].z;						
		}
	}
	
	//setup side planes
	m_kSide[0].Set(Vector3(0,0,1),	m_kMax.z);
	m_kSide[1].Set(Vector3(0,0,-1),	m_kMin.z);
	m_kSide[2].Set(Vector3(1,0,0),	m_kMax.x);
	m_kSide[3].Set(Vector3(-1,0,0),	m_kMin.x);
	m_kSide[4].Set(Vector3(0,1,0),	m_kMax.y);
	m_kSide[5].Set(Vector3(0,-1,0),	m_kMin.y);
}

int AABB::LongestAxis()
{
	int l = 0;
	
	for(int i = 1;i<3;i++)
 		if( fabs(m_kMin[i] - m_kMax[i]) > fabs(m_kMin[l] - m_kMax[l]) )
			l = i;

			
	return l;
}

bool AABB::TestSphere(const Vector3& kPos,float fRadius)
{
	//not exact SPHERE VS AABB test, more like AABB VS AABB, but its faster thou ;)

	if(kPos.x - fRadius > m_kMax.x) return false;
	if(kPos.x + fRadius < m_kMin.x) return false;

	if(kPos.y - fRadius > m_kMax.y) return false;
	if(kPos.y + fRadius < m_kMin.y) return false;

	if(kPos.z - fRadius > m_kMax.z) return false;
	if(kPos.z + fRadius < m_kMin.z) return false;
	
	return true;
}

bool AABB::TestLine(const Vector3& kP1,const Vector3& kP2)
{
	static Vector3 kColPos;
	
	//test against planes
	for(int i = 0;i<6;i++)
	{
		if(m_kSide[i].LineTest(kP1,kP2,&kColPos))
		{
			switch(i)
			{
				case 0:
				case 1:
					if(kColPos.x > m_kMin.x && kColPos.x < m_kMax.x &&
						kColPos.y > m_kMin.y && kColPos.y < m_kMax.y)
						return true;
					break;
			
				case 2:
				case 3:
					if(kColPos.z > m_kMin.z && kColPos.z < m_kMax.z &&
						kColPos.y > m_kMin.y && kColPos.y < m_kMax.y)
						return true;
					break;
			
				case 4:
				case 5:
					if(kColPos.z > m_kMin.z && kColPos.z < m_kMax.z &&
						kColPos.x > m_kMin.x && kColPos.x < m_kMax.x)
						return true;
					break;
			}
		}
	}
	
	return false;
}

//---- Triangle



bool Triangle::TestSphere(const Vector3& kPos,float fRadius,Vector3* pkTestPos)
{
	static Vector3 kDistance;
	
	kDistance = m_kPlane.m_kNormal * fRadius;

	//test against polygon plane
	if(m_kPlane.LineTest(kPos + kDistance,kPos - kDistance,pkTestPos))
		if(TestSides(*pkTestPos))
			return true;

	
	//do edge tests
	static bool bDidcollide;
	static int p1,p2;
	static float fClosest;	
	static float fDistance;
	static Vector3 kTempPos;
	
	bDidcollide 	= false;
	fClosest 		= 99999999;	
	
	for(int i = 0;i<3;i++)
	{
		switch(i)
		{
			case 0:
				p1 = 0;
				p2 = 1;
				break;
			case 1:
				p1 = 1;
				p2 = 2;
				break;
			case 2:
				p1 = 2;
				p2 = 0;
				break;
		}		
	
 		if(TestLineVSSphere(kPos,fRadius,m_kVerts[p1], m_kVerts[p2],&kTempPos))
		{
 			fDistance = kPos.DistanceTo(kTempPos);
			if( fDistance < fClosest)
			{
				fClosest = fDistance;
				*pkTestPos = kTempPos;
				bDidcollide = true;
			}
		}		
	}
	
	if(bDidcollide)
		return true;
	
	
	//test verts
	fClosest 	= 99999999;
	bDidcollide = false;
	
	for(int i = 0;i< 3;i++)
	{
		fDistance = kPos.DistanceTo(m_kVerts[i]);
		if(fDistance < fRadius)
			if(fDistance < fClosest)
			{
				*pkTestPos = m_kVerts[i];
				bDidcollide = true;
				fClosest		= fDistance;
			}	
	}
	
	
	
	return bDidcollide;

}

bool Triangle::TestLineVSSphere(const Vector3& kPos,float fRadius,const Vector3& kP1,const Vector3& kP2,Vector3* kTestPos)
{
	static Vector3 kDir,c,k;
	static float d,cdis,kdis,fDistance;
	

	kDir = kP2 - kP1;
	c=kPos - kP1;		

	d = kDir.Unit().Dot(c);
	if(d < 0)
		return false;
	if(d > kDir.Length())
		return false;
	
	kDir.Normalize();		
	k=kDir.Proj(c);		
	cdis=c.Length();
	kdis=k.Length();
	fDistance = (float) sqrt((cdis*cdis)-(kdis*kdis));
	
	if(fDistance < fRadius)
	{			
		*kTestPos = kP1 + k;		
		return true;
	}		
	
	return false;	
}

bool Triangle::TestSides(const Vector3& kPos)
{
	static Vector3 vert0p;
	static Vector3 vert1p;
	static Vector3 vert2p;

	vert0p = m_kVerts[0] - kPos;
	vert1p = m_kVerts[1] - kPos;
	float d = vert0p.Cross(vert1p).Dot(m_kPlane.m_kNormal);
	if (d < 0) return false;
	vert2p = m_kVerts[2] - kPos;
	d = vert1p.Cross(vert2p).Dot(m_kPlane.m_kNormal);
	if (d < 0) return false;
	d = vert2p.Cross(vert0p).Dot(m_kPlane.m_kNormal);
	if (d < 0) return false;
	return true;

// 	static Vector3 e10,e20,vp;
// 	static float a,b,c,ac_bb,d,e;	
// 	
// 	e10= m_kVerts[1]-m_kVerts[0];
// 	e20= m_kVerts[2]-m_kVerts[0];
// 	
// 	a 	= e10.Dot(e10);
// 	b 	= e10.Dot(e20);
// 	c 	= e20.Dot(e20);	
// 	ac_bb=(a*c)-(b*b);
// 		
// 	vp.Set(kPos.x-m_kVerts[0].x, kPos.y-m_kVerts[0].y, kPos.z-m_kVerts[0].z);
// 	
// 	d = vp.Dot(e10);
// 	e = vp.Dot(e20);
// 	
// 	float x = (d*c)-(e*b);
// 	float y = (e*a)-(d*b);
// 	float z = x+y-ac_bb;
// 	
// 	return (( ((unsigned int &)z) & ~(((unsigned int &)x)|((unsigned int &)y)) ) & 0x80000000) != 0;
}

bool Triangle::TestLine(const Vector3& kP1,const Vector3& kP2,Vector3* pkTestPos)
{
	if(m_kPlane.LineTest(kP1 , kP2 ,pkTestPos))
		if(TestSides(*pkTestPos))
			return true;	

	return false;
}

