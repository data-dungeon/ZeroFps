#include "waterobject.h"

WaterObject::WaterObject(int iSize,int iStep,const char* acTexture)
{
	m_kName="WaterObject";
	m_bSave=false;

	AddProperty("WaterRenderProperty");
	(static_cast<WaterRenderProperty*>(GetProperty("WaterRenderProperty")))->SetProperty(iSize,iStep,acTexture);

}

void WaterObject::SetTexture(const char* acTexture)
{
	WaterRenderProperty *water = static_cast<WaterRenderProperty*>(GetProperty("WaterRenderProperty"));
	water->SetTexture(acTexture);
}



