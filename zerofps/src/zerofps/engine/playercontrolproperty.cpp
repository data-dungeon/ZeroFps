#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(HeightMap* pkHeightMap,Render *pkRender) {
	m_pkMap=pkHeightMap;
	m_pkRender=pkRender;
	strcpy(m_acName,"PlayerControlProperty");
};


void PlayerControlProperty::Update() {
	m_pkObject->GetPos().x+=0.1;
	m_pkObject->GetPos().z+=0.1;	
	m_pkObject->GetPos().y=m_pkMap->Height(m_pkObject->GetPos().x,m_pkObject->GetPos().z);


	Vector3 normal=m_pkMap->Tilt(m_pkObject->GetPos().x,m_pkObject->GetPos().z);
	m_pkRender->Line(m_pkObject->GetPos(),m_pkObject->GetPos()+normal);
	

};






