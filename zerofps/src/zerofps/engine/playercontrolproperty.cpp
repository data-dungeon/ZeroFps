#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty() {
	strcpy(m_acName,"PlayerControl");
};


void PlayerControlProperty::Update() {
	m_pkObject->GetPos().x+=0.01;
	

};






