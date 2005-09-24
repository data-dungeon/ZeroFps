#include "sceneaabb.h"
#include "entitymanager.h"


//node
SceneAABBNode::SceneAABBNode(vector<Entity*>*	pkEntitys,SceneAABBNode* pkParent)
{
	m_pkChild1		=	NULL;
	m_pkChild2		=	NULL;		

	m_bInFrustum	=	true;

	//save parent for future use
	m_pkParentNode	=	pkParent;
	
	Build(pkEntitys);
}


void SceneAABBNode::Build(vector<Entity*>*	pkEntitys)
{
	//calculate AABB size
	SetAABBSize(pkEntitys);


	//do we want to create childs
	if(pkEntitys->size() > 4 )
	{
		//in wich axis shuld we split the mesh?
		int iLongestAxis = FindLongestAxis();	

		//find center point
		Vector3 kCenter(0,0,0);				
		int iSize = pkEntitys->size();
		for(int i = 0;i<iSize;i++)
			kCenter += (*pkEntitys)[i]->GetWorldPosV();
		kCenter *= 1.0 / iSize;		
		
		
		//split polygons
		vector<Entity*>	kChildBucket1;
		vector<Entity*>	kChildBucket2;			
		for(int i = 0;i<iSize;i++)
		{
			if( (*pkEntitys)[i]->GetWorldPosV()[iLongestAxis] > kCenter[iLongestAxis])
				kChildBucket2.push_back((*pkEntitys)[i]);
			else
				kChildBucket1.push_back((*pkEntitys)[i]);				
		}		

		//if one of the buckets are empty create a leaf node
		if(kChildBucket1.empty() || kChildBucket2.empty())
		{
// 			cout<<"created leaf size: "<<pkEntitys->size()<<endl;		
			m_kEntitys = *pkEntitys;	
			BindEntitys();	
			return;		
		}

		//create childs
		m_pkChild1 = new SceneAABBNode(&kChildBucket1,this);
		m_pkChild2 = new SceneAABBNode(&kChildBucket2,this);
	}
	else
	{
// 		cout<<"created leaf size: "<<pkEntitys->size()<<endl;
	
		m_kEntitys = *pkEntitys;		
		BindEntitys();
	}

}

void SceneAABBNode::Update(Entity* pkEntity,bool bOwner)
{	
	//if entitys is still inside the AABB , then nothing needs to be done
	if(EntityInsideNode(pkEntity))
	{
		//if we are the previus owner, dont do anything
		if(bOwner)
		{
// 			cout<<"just updating AABB"<<endl;
// 			SetAABBSize(&m_kEntitys);
			return;
		}
		
		//if we're not rebuild ourself
		RebuildTree();
		
		return;
	}


	//if entity was not inside this node, and we have a parent send update to parent.
	if(m_pkParentNode)
	{
		cout<<"entity was outside , trying parent"<<endl;
		m_pkParentNode->Update(pkEntity,false);
		return;
	}
	
	//if entity was not inside this node, and we are the root node, rebuild tree
	cout<<"entity was outside, where root, rebuilding"<<endl;
	RebuildTree();
}

void SceneAABBNode::BindEntitys()
{
	int iSize = m_kEntitys.size();
	for(int i = 0;i<iSize;i++)
	{
		m_kEntitys[i]->m_pkSceneAABBNode = this;	
	}

}

void SceneAABBNode::RebuildTree()
{
	vector<Entity*>	kEntitys;
	GetAllEntitys(&kEntitys);

	delete m_pkChild1;
	delete m_pkChild2;

	m_pkChild1	=	NULL;
	m_pkChild2	=	NULL;		

	m_kEntitys.clear();

	Build(&kEntitys);
}

void SceneAABBNode::GetAllEntitys(vector<Entity*>*	pkEntitys)
{
	int iSize = m_kEntitys.size();
	for(int i = 0;i<m_kEntitys.size();i++)
		pkEntitys->push_back(m_kEntitys[i]);

	if(m_pkChild1)
		m_pkChild1->GetAllEntitys(pkEntitys);
		
	if(m_pkChild2)
		m_pkChild2->GetAllEntitys(pkEntitys);		
}

SceneAABBNode::~SceneAABBNode()
{
	if(m_pkChild1)
		delete m_pkChild1;
		
	if(m_pkChild2)
		delete m_pkChild2;

}

int SceneAABBNode::FindLongestAxis()
{
	int l = 0;
	
	for(int i = 1;i<3;i++)
 		if( fabs(m_kAABBMin[i] - m_kAABBMax[i]) > fabs(m_kAABBMin[l] - m_kAABBMax[l]) )
			l = i;

			
	return l;
}

void SceneAABBNode::SetAABBSize(vector<Entity*>*	pkEntitys)
{
	static Vector3 kTempMin;
	static Vector3 kTempMax;
	static Vector3 kTempRadius;
	
	m_kAABBMin.Set(999999999,99999999,99999999);
	m_kAABBMax.Set(-999999999,-99999999,-99999999);


	//iterator trough all entitys, finding the most extreme x y z cordinats
	int iSize = pkEntitys->size();
	for(int i = 0;i<iSize;i++)
	{
		kTempRadius.Set( (*pkEntitys)[i]->GetRadius(),(*pkEntitys)[i]->GetRadius(),(*pkEntitys)[i]->GetRadius());
		kTempMin = (*pkEntitys)[i]->GetWorldPosV() - kTempRadius;
		kTempMax = (*pkEntitys)[i]->GetWorldPosV() + kTempRadius;
	
		//min
		if( kTempMin.x < m_kAABBMin.x)
			m_kAABBMin.x = kTempMin.x;
		if( kTempMin.y < m_kAABBMin.y)
			m_kAABBMin.y = kTempMin.y;
		if( kTempMin.z < m_kAABBMin.z)
			m_kAABBMin.z = kTempMin.z;

		//max
		if( kTempMax.x > m_kAABBMax.x)
			m_kAABBMax.x = kTempMax.x;
		if( kTempMax.y > m_kAABBMax.y)
			m_kAABBMax.y = kTempMax.y;
		if( kTempMax.z > m_kAABBMax.z)
			m_kAABBMax.z = kTempMax.z;			
	
	}
}

void SceneAABBNode::GetAABBList(vector<AABBGraphInfo>* pkAABBList)
{
	AABBGraphInfo kNew;
	kNew.m_kMin = m_kAABBMin;
	kNew.m_kMax = m_kAABBMax;
	kNew.m_bLeaf = !m_pkChild1;	
	kNew.m_bInFrustum = m_bInFrustum;	
	pkAABBList->push_back(kNew);

	if(m_pkChild1)
	{
		m_pkChild1->GetAABBList(pkAABBList);
		m_pkChild2->GetAABBList(pkAABBList);		
	}
}

bool SceneAABBNode::EntityInsideNode(Entity* pkEnt)
{
	static Vector3 kPos;
	static float fRadius;
	
	kPos = pkEnt->GetWorldPosV();
	fRadius= pkEnt->GetRadius();

	if(kPos.x +fRadius > m_kAABBMax.x)	return false;
	if(kPos.x -fRadius < m_kAABBMin.x)	return false;
	if(kPos.y +fRadius > m_kAABBMax.y)	return false;
	if(kPos.y -fRadius < m_kAABBMin.y)	return false;
	if(kPos.z +fRadius > m_kAABBMax.z)	return false;
	if(kPos.z -fRadius < m_kAABBMin.z)	return false;

	return true;
}

bool SceneAABBNode::IsInsideNode(const Vector3& kPos)
{
	if(kPos.x > m_kAABBMax.x)	return false;
	if(kPos.x < m_kAABBMin.x)	return false;
	if(kPos.y > m_kAABBMax.y)	return false;
	if(kPos.y < m_kAABBMin.y)	return false;
	if(kPos.z > m_kAABBMax.z)	return false;
	if(kPos.z < m_kAABBMin.z)	return false;

	return true;
}


void SceneAABBNode::RemoveEntity(Entity* pkEntity)
{
	bool bFound = false;

	for(vector<Entity*>::iterator it = m_kEntitys.begin();it!=m_kEntitys.end();it++)
	{
		if( (*it) == pkEntity)
		{
			m_kEntitys.erase(it);
			bFound = true;
			break;
		}
	}
	
	if(!bFound)
		cout<<"WARNING  SceneAABBNode::RemoveEntity: did not find entity"<<endl;
	
	pkEntity->m_pkSceneAABBNode = NULL;
	
	if(m_pkParentNode)
		m_pkParentNode->RebuildTree();
	else
		RebuildTree();
}

void SceneAABBNode::InsertEntity(Entity* pkEntity)
{
	if(m_pkChild1 && m_pkChild1->IsInsideNode(pkEntity->GetWorldPosV()))
	{
// 		cout<<"insertet to child 1"<<endl;
		m_pkChild1->InsertEntity(pkEntity);				
		return;
	}
	
	if(m_pkChild2 && m_pkChild2->IsInsideNode(pkEntity->GetWorldPosV()))
	{
// 		cout<<"insertet to child 2"<<endl;			
		m_pkChild2->InsertEntity(pkEntity);
		return;
	}
		
	
	m_kEntitys.push_back(pkEntity);
		
// 	cout<<"no child to handle this entity, rebuilding current node "<<m_kEntitys.size()<<endl;
		
	RebuildTree();
}


void SceneAABBNode::GetRenderPropertysInFrustum(vector<Property*>* pkPropertys,Frustum* pkFrustum)
{
	//cube in frustum?
	if(pkFrustum->CubeInFrustum(m_kAABBMin,m_kAABBMax))
	{
		int iSize = m_kEntitys.size();
		for(int i = 0;i<iSize;i++)
		{
			m_kEntitys[i]->GetPropertys(pkPropertys,PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);			
		}
	
	
		if(m_pkChild1)
			m_pkChild1->GetRenderPropertysInFrustum(pkPropertys,pkFrustum);
		if(m_pkChild2)
			m_pkChild2->GetRenderPropertysInFrustum(pkPropertys,pkFrustum);	
	}
}

void SceneAABBNode::FrustumCulling(Frustum*	pkFrustum)
{	
	//dont test
	if(!pkFrustum)
	{
		m_bInFrustum = false;
	
		if(m_pkChild1)
			m_pkChild1->FrustumCulling(NULL);
		if(m_pkChild2)
			m_pkChild2->FrustumCulling(NULL);		
	}
	else
	{
		//cube in frustum?
		if(pkFrustum->CubeInFrustum(m_kAABBMin,m_kAABBMax))
		{
			m_bInFrustum = true;
		
			if(m_pkChild1)
				m_pkChild1->FrustumCulling(pkFrustum);
			if(m_pkChild2)
				m_pkChild2->FrustumCulling(pkFrustum);	
		}
		else
		{
			m_bInFrustum = false;
		
			if(m_pkChild1)
				m_pkChild1->FrustumCulling(NULL);
			if(m_pkChild2)
				m_pkChild2->FrustumCulling(NULL);			
		}
	}	
}

//TREE

SceneAABBTree::SceneAABBTree(ZSSEntityManager*	pkEntityManager)
{
	m_pkRoot = NULL;
	m_pkEntityManager = pkEntityManager;

}

void SceneAABBTree::Clear()
{
	if(m_pkRoot)
		delete m_pkRoot;

	m_pkRoot = NULL;
}

void SceneAABBTree::RebuildTree()
{
	Clear();
	
	if(!m_pkEntityManager->GetZoneEntity())
		return;

	vector<Entity*>	kEntitys;
	
	Entity* pkWorldEntity = m_pkEntityManager->GetZoneEntity();
	pkWorldEntity->GetAllEntitys(&kEntitys,true,false);

	cout<<"creating new graph, totalt size "<<kEntitys.size()<<endl;

	
	m_pkRoot = new SceneAABBNode(&kEntitys,NULL);
}

void SceneAABBTree::InsertEntity(Entity* pkEntity)
{
	if(!m_pkRoot)
	{
		//RebuildTree();
		vector<Entity*> kEntitys;
		kEntitys.push_back(pkEntity);
		m_pkRoot = new SceneAABBNode(&kEntitys,NULL);
		return;
	}
	
	if(pkEntity->m_pkSceneAABBNode)
		return;
	
	m_pkRoot->InsertEntity(pkEntity);
}

void SceneAABBTree::GetAABBList(vector<AABBGraphInfo>* pkAABBList)
{
	if(m_pkRoot)
		m_pkRoot->GetAABBList(pkAABBList);
}

void SceneAABBTree::FrustumCulling(Frustum*	pkFrustum)
{
	if(m_pkRoot)
		m_pkRoot->FrustumCulling(pkFrustum);

}

void SceneAABBTree::GetRenderPropertysInFrustum(vector<Property*>* pkPropertys,Frustum* pkFrustum)
{
	if(m_pkRoot)
		m_pkRoot->GetRenderPropertysInFrustum(pkPropertys,pkFrustum);
	
}



