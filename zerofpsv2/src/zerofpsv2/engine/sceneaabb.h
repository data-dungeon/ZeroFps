#ifndef _SCEENAABB_H_
#define _SCEENAABB_H_

#include "entity.h"



class ZSSEntityManager;


class AABBGraphInfo
{
	public:
		Vector3	m_kMin;
		Vector3	m_kMax;
		
		bool		m_bLeaf;
		bool		m_bInFrustum;
};

class SceneAABBNode
{
	private:
		Vector3			m_kAABBMin;
		Vector3			m_kAABBMax;
	
		SceneAABBNode*	m_pkParentNode;
		SceneAABBNode*	m_pkChild1;
		SceneAABBNode*	m_pkChild2;

		vector<Entity*>	m_kEntitys;
	
		bool					m_bInFrustum;

		void SetAABBSize(vector<Entity*>*	pkEntitys);
		int  FindLongestAxis();

		bool IsInsideNode(const Vector3& kPos);
		bool EntityInsideNode(Entity* pkEnt);
		void Build(vector<Entity*>*	kEntitys);
		void BindEntitys();
		
		
		void FrustumCulling(Frustum*	pkFrustum);	
		void GetRenderPropertysInFrustum(vector<Property*>* pkPropertys,Frustum* pkFrustum);
	public:
		SceneAABBNode(vector<Entity*>*	kEntitys,SceneAABBNode* pkParent);
		~SceneAABBNode();

		void GetAABBList(vector<AABBGraphInfo>* pkAABBList);
		void GetAllEntitys(vector<Entity*>*	pkEntitys);
		void RebuildTree();

		void InsertEntity(Entity* pkEntity);		
		void RemoveEntity(Entity* pkEntity);
		void Update(Entity* pkEntity,bool bOwner = true);
		

	friend class SceneAABBTree;
};


class SceneAABBTree
{
	private:
		ZSSEntityManager*		m_pkEntityManager;	
		
		SceneAABBNode*			m_pkRoot;


	public:
		SceneAABBTree(ZSSEntityManager*	pkEntityManager);
	
		void RebuildTree();
		void Clear();


		void GetAABBList(vector<AABBGraphInfo>* pkAABBList);		
		void InsertEntity(Entity* pkEntity);
		
		void FrustumCulling(Frustum*	pkFrustum);
		void GetRenderPropertysInFrustum(vector<Property*>* pkPropertys,Frustum* pkFrustum);
};


#endif

