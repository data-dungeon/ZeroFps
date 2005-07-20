#include "zfversion.h"


ZFVersion::ZFVersion()		
{ 
	m_iMajor=0; 
	m_iMinor=0; 
	m_iBuild=0; 
}

ZFVersion::ZFVersion(int iMajor, int iMinor, int iBuild)
{ 
	m_iMajor=iMajor; 
	m_iMinor=iMinor; 
	m_iBuild=iBuild; 
}

ZFVersion::~ZFVersion()	{ }

void ZFVersion::Set(int iMajor, int iMinor, int iBuild)	
{ 
	m_iMajor=iMajor; 
	m_iMinor=iMinor; 
	m_iBuild=iBuild; 
}

bool ZFVersion::operator==(const ZFVersion &kOther)	const
{
	if(m_iMajor == kOther.m_iMajor &&
		m_iMinor == kOther.m_iMinor &&
		m_iBuild == kOther.m_iBuild)
		return true;
	
	return false;
}

bool ZFVersion::operator!=(const ZFVersion &kOther)	const
{
	return !(kOther == *this); 
}
