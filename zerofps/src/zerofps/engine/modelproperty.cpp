#include "modelproperty.h"


ModelProperty::ModelProperty() {

}


void ModelProperty::Update() {
	glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
	glutSolidSphere(5,10,10);
	

}





