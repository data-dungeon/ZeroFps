#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(HeightMap* pkHeightMap) {
	m_pkMap=pkHeightMap;
	strcpy(m_acName,"PlayerControlProperty");
};


void PlayerControlProperty::Update() {
	m_pkObject->GetPos().x+=0.1;
	m_pkObject->GetPos().z+=0.1;	
	m_pkObject->GetPos().y=m_pkMap->Height(m_pkObject->GetPos().x,m_pkObject->GetPos().z);

};






