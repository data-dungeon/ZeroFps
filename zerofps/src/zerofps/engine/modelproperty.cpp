#include "modelproperty.h"
#include "object.h"


ModelProperty::ModelProperty() {
	strcpy(m_acName,"ModelProperty");
	m_fRadius = 1.0;
	m_iSlices = 5;
	m_iStacks = 5;
	m_kColor.Set(1,1,1);
}


void ModelProperty::Update() {
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	glColor3f(m_kColor.x,m_kColor.y,m_kColor.z);
	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glutSolidSphere(m_fRadius, m_iSlices, m_iStacks);
	glPopMatrix();
	glPopAttrib();
}


Property* Create_ModelProperty()
{
	return new ModelProperty;
}




