#include "aabbtree.h"


//---- AABBTree
AABBTree::AABBTree()
{
	m_pkRootNode = NULL;

	m_kInfo.m_iMaxDepth 		= 100;
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
								float fScale,
								const Vector3& kPos
							)
{

	//create triangle list
	vector<Triangle>	kTriangles;	
	Triangle kTemp;
	
	int iSize = m_pkFaces->size();
	for(int f= 0;f<iSize;f++)
	{
		//setup vertex
		kTemp.m_kVerts[0] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[0]] );
		kTemp.m_kVerts[1] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[1]] );
		kTemp.m_kVerts[2] =  kRotation.VectorTransform( (*m_pkVertex)[(*m_pkFaces)[f].iIndex[2]] );	
		kTemp.m_kVerts[0] *= fScale;
		kTemp.m_kVerts[1] *= fScale;
		kTemp.m_kVerts[2] *= fScale;
 		kTemp.m_kVerts[0] += kPos;
 		kTemp.m_kVerts[1] += kPos;
 		kTemp.m_kVerts[2] += kPos;
		
		
		//setup triangles plane
		kTemp.m_kPlane.Set(kTemp.m_kVerts[0],kTemp.m_kVerts[1],kTemp.m_kVerts[2]);
		
		kTriangles.push_back(kTemp);
	}

	//remove old root node if any
	if(m_pkRootNode)
		delete m_pkRootNode;
		
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
		int iLongestAxis = m_kBox.LongestAxis();	
		Vector3 kGeoAvrageCenter(0,0,0);
		
		//calculate geometrix avrage
		int iSize = pkTriangles->size();
		for(int i = 0;i<iSize;i++)
			kGeoAvrageCenter += (*pkTriangles)[i].Middle();
		kGeoAvrageCenter *= 1.0 / iSize;
			
		//split polygons
		vector<Triangle>	kBucket1;
		vector<Triangle>	kBucket2;			
		for(int i = 0;i<iSize;i++)
		{
			if( (*pkTriangles)[i].Middle()[iLongestAxis] > kGeoAvrageCenter[iLongestAxis])
				kBucket2.push_back((*pkTriangles)[i]);
			else
				kBucket1.push_back((*pkTriangles)[i]);				
		}
		
		
		//create childs
		m_pkChild1 = new AABBNode();
		m_pkChild2 = new AABBNode();
		
		m_pkChild1->Create(&kBucket1,iDepth+1,kInfo);
		m_pkChild2->Create(&kBucket2,iDepth+1,kInfo);
	}
	else
	{		
		//cout<<"bucket size"<<pkTriangles->size()<<endl;
	
		m_kTriangles = *pkTriangles;
		
		m_pkChild1=NULL;
		m_pkChild2=NULL;
	}

}

void AABBNode::GetAABBList(vector<AABB>* pkAABBList)
{
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
	else
		return false;
}

bool AABBNode::TestSphere(const Vector3& kPos,float fRadius,Vector3* pkTestPos)
{
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
				
			//both tests was true, check wich was closest
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
			static Vector3 kClosestPos;	
			
			bHaveCollided = false;
			fClosest = 999999999;
			
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
// 						*pkTestPos = kTempPos;
						kClosestPos	 = kTempPos;
					}
				}			
			}

			if(bHaveCollided)
			{
				*pkTestPos = kClosestPos;
				return true;
			
			}
			else
				return false;
// 			return bHaveCollided;
		}
	}
	else
		return false;
}





//---- AABB

void AABB::Set(vector<Triangle>*	pkTriangles)
{
	m_kMin.Set(999999999,99999999,99999999);
	m_kMax.Set(-999999999,-99999999,-99999999);

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
	m_kSide[0].Set(Vector3(0,0,1),m_kMax.z);
	m_kSide[1].Set(Vector3(0,0,-1),m_kMin.z);
	m_kSide[2].Set(Vector3(1,0,0),m_kMax.x);
	m_kSide[3].Set(Vector3(-1,0,0),m_kMin.x);
	m_kSide[4].Set(Vector3(0,1,0),m_kMax.y);
	m_kSide[5].Set(Vector3(0,-1,0),m_kMin.y);
}

int AABB::LongestAxis()
{
	int l = 0;
	
	for(int i = 1;i<3;i++)
	{
 		if( fabs(m_kMin[i] - m_kMax[i]) > fabs(m_kMin[l] - m_kMax[l]) )
			l = i;
	}

	return l;
}

bool AABB::TestSphere(const Vector3& kPos,float fRadius)
{
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

// 	if(kP1.x < m_kMin.x && kP2.x < m_kMin.x || kP1.x > m_kMax.x && kP2.x > m_kMax.x ) return false;
// 	if(kP1.y < m_kMin.y && kP2.y < m_kMin.y || kP1.y > m_kMax.y && kP2.y > m_kMax.y ) return false;
// 	if(kP1.z < m_kMin.z && kP2.z < m_kMin.z || kP1.z > m_kMax.z && kP2.z > m_kMax.z ) return false;
	
	
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

	if(m_kPlane.LineTest(kPos + kDistance,kPos - kDistance,pkTestPos))
	{
		if(TestSides(*pkTestPos))
		{
			return true;
		}
	}

	
	//do edge tests
	static bool didcollide;
	static float closest;
	static int p1,p2;
	static float d;
	static Vector3 kTempPos;
	
	didcollide = false;
	closest = 99999999;	
	
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
	
		if(TestLineVSSphere(kPos,fRadius,m_kVerts[p1], m_kVerts[p2],pkTestPos))
		{
			d = kPos.DistanceTo(*pkTestPos);
			if( d < closest)
			{
				closest = d;
				didcollide = true;
			}
		}		
	}
	
	if(didcollide)
		return true;
	
	
	//test verts
	d = 99999999;
	didcollide = false;
	for(int i = 0;i< 3;i++)
	{
		d = kPos.DistanceTo(m_kVerts[i]);
		if(d < fRadius)
			if(d < closest)
			{
				*pkTestPos = m_kVerts[i];
				didcollide = true;
			}	
	}
	
	if(didcollide)
	{
		return true;
	}	
	
	return false;	

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
	static Vector3 e10,e20,vp;
	static float a,b,c,ac_bb,d,e;	
	
	e10=m_kVerts[1]-m_kVerts[0];
	e20=m_kVerts[2]-m_kVerts[0];
	a = e10.Dot(e10);
	b = e10.Dot(e20);
	c = e20.Dot(e20);
	ac_bb=(a*c)-(b*b);
	vp.Set(kPos.x-m_kVerts[0].x, kPos.y-m_kVerts[0].y, kPos.z-m_kVerts[0].z);
	d = vp.Dot(e10);
	e = vp.Dot(e20);
	
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;
	return (( ((unsigned int &)z) & ~(((unsigned int &)x)|((unsigned int &)y)) ) & 0x80000000) != 0;

}

bool Triangle::TestLine(const Vector3& kP1,const Vector3& kP2,Vector3* pkTestPos)
{
	if(m_kPlane.LineTest(kP1 , kP2 ,pkTestPos))
		if(TestSides(*pkTestPos))
			return true;	

	return false;
}

