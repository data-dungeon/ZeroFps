#include "../../zerofps/basic/basicmath.pkg"
#include "madexport.h"

Mad_CoreBone::Mad_CoreBone()
{
	strcpy(m_acName, "empty");
	m_iParent = -2;
	m_kPosition.Set(0,0,0);
	m_kRotation.Set(0,0,0);
}

Mad_CoreBone::~Mad_CoreBone()
{

}

void Mad_CoreBone::Clear(void)
{
	strcpy(m_acName, "clear");
	m_iParent = -2;
	m_kPosition.Set(0,0,0);
	m_kRotation.Set(0,0,0);
}

void Mad_CoreBone::operator=(const Mad_CoreBone& kOther)
{
	strcpy(m_acName, kOther.m_acName);
	m_iParent = kOther.m_iParent;
	m_kPosition = kOther.m_kPosition;
	m_kRotation = kOther.m_kRotation;

}




Mad_CoreVertexFrame::Mad_CoreVertexFrame()
{

}

Mad_CoreVertexFrame::~Mad_CoreVertexFrame()
{

}

void Mad_CoreVertexFrame::operator=(const Mad_CoreVertexFrame& kOther)
{
	akVertex = kOther.akVertex;
	akNormal = kOther.akNormal;
}


