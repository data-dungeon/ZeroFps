#include "modelproperty.h"


ModelProperty::ModelProperty() {
	strcpy(m_acName,"ModelProperty");
}


void ModelProperty::Update() {
	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glutSolidSphere(5,10,10);
	glPopMatrix();

}





