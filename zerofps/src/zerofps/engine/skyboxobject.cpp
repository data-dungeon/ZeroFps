#include "skyboxobject.h"

SkyBoxObject::SkyBoxObject(const char* acHor,const char* acTop) {
	m_bStatic=true;
	AddProperty(new SkyBoxRenderProperty(acHor,acTop));
}

void SkyBoxObject::SetTexture(const char* acHor,const char* acTop)
{
	SkyBoxRenderProperty *sky = static_cast<SkyBoxRenderProperty*>(GetProperty("SkyBoxRenderProperty"));
	sky->SetTexture(acHor,acTop);
}

void SkyBoxObject::SetRotate(Vector3 kRotate)
{
	SkyBoxRenderProperty *sky = static_cast<SkyBoxRenderProperty*>(GetProperty("SkyBoxRenderProperty"));
	sky->SetRotate(kRotate);
}