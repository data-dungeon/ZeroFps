#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty() {
	strcpy(m_acName,"PlayerControlProperty");
};


void PlayerControlProperty::Update() {
	m_pkObject->GetPos().x+=0.1;
	

};






