#include "modelproperty.h"
#include "object.h"


ModelProperty::ModelProperty() {
	strcpy(m_acName,"ModelProperty");
}


void ModelProperty::Update() {
	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glutSolidSphere(1,5,5);
	glPopMatrix();

}





