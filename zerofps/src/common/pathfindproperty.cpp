// pathfindproperty.cpp: implementation of the PathFindProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "pathfindproperty.h"
#include "../zerofps/engine/pathfind.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathFindProperty::PathFindProperty()
{
	strcpy(m_acName,"PathFindProperty");
	m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));
	m_kNextSqr.x = m_kEndSqr.x = m_pkObject->GetPos().x;
	m_kNextSqr.y = m_kEndSqr.y = m_pkObject->GetPos().z;
}

PathFindProperty::~PathFindProperty()
{

}

void PathFindProperty::Update()
{
	float fObjX = m_pkObject->GetPos().x;
	float fObjZ = m_pkObject->GetPos().z;
	m_pkMap->GetMapXZ(fObjX,fObjZ);

	if(AtDest())
	{
		if(m_pkPathFind->GetNextStep(m_kNextSqr.x, m_kNextSqr.y) == false)
		{
			m_pkPathFind->Rebuild(fObjX, fObjZ, m_kEndSqr.x, m_kEndSqr.y);
		}
	}
}

void PathFindProperty::SetDstSqr(int x, int z)
{
	m_kEndSqr.x = x;
	m_kEndSqr.y = z;
}

void PathFindProperty::SetPathFindObject(PathFind* pkPathFind)
{
	m_pkPathFind = pkPathFind;
}

bool PathFindProperty::AtDest()
{
	float fObjX = m_pkObject->GetPos().x;
	float fObjZ = m_pkObject->GetPos().z;

	m_pkMap->GetMapXZ(fObjX, fObjZ);

	return (fabs(fObjX-m_kEndSqr.x) <= 1.5f &&
		    fabs(fObjZ-m_kEndSqr.y) <= 1.5f);
}

void PathFindProperty::GetNextSqr(int &x, int &z)
{
	x = m_kNextSqr.x;
	z = m_kNextSqr.y;
}
