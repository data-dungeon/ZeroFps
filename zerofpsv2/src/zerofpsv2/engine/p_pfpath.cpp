#include "p_pfpath.h"
#include "entity.h"

P_PfPath::P_PfPath()
{
	strcpy(m_acName,"P_PfPath");
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER;

}

P_PfPath::~P_PfPath()
{

}

void P_PfPath::Update()
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

void P_PfPath::Save(ZFIoInterface* pkFile)
{

}

void P_PfPath::Load(ZFIoInterface* pkFile)
{

}

void P_PfPath::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfPath::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{

}

vector<PropertyValues> P_PfPath::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	return kReturn;
}

void P_PfPath::SetPath(vector<Vector3> kPath)
{
	m_kPath = kPath;
	m_iNextGoal = 0;
}



Property* Create_P_PfPath()
{
	return new P_PfPath;
}

