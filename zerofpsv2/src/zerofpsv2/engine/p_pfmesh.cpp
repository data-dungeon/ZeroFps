#include "p_pfmesh.h"
#include "entity.h"

P_PfMesh::P_PfMesh()
{
	strcpy(m_acName,"P_PfMesh");
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER;

}

P_PfMesh::~P_PfMesh()
{

}

void P_PfMesh::Update()
{
	if(m_kPath.size() == 0)
		return;

	// Get Distance to next goal.
	Vector3 kPos = m_pkObject->GetWorldPosV();
	Vector3 kGoal = m_kPath[m_iNextGoal];

	Vector3 kdiff = kGoal - kPos;
	float fdist = kdiff.Length();
	if(fdist < 0.5) {
		m_pkObject->SetWorldPosV(kGoal);
		m_iNextGoal++;
		if(m_iNextGoal == m_kPath.size()) 
			m_kPath.clear();
		return;
		}

	
	kdiff.Normalize();
	kPos += (kdiff * 0.1);
	m_pkObject->SetWorldPosV(kPos);
	printf("Have A Path\n");
}

void P_PfMesh::Save(ZFIoInterface* pkFile)
{

}

void P_PfMesh::Load(ZFIoInterface* pkFile)
{

}

void P_PfMesh::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfMesh::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{

}

vector<PropertyValues> P_PfMesh::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	return kReturn;
}

void P_PfMesh::SetPath(vector<Vector3> kPath)
{
	m_kPath = kPath;
	m_iNextGoal = 0;
}



Property* Create_P_PfMesh()
{
	return new P_PfMesh;
}

